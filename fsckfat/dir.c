/*
 * Copyright (C) 1995, 1996, 1997 Wolfgang Solfrank
 * Copyright (c) 1995 Martin Husemann
 * Some structure declaration borrowed from Paul Popelka
 * (paulp@uts.amdahl.com), see /sys/msdosfs/ for reference.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by Martin Husemann
 *  and Wolfgang Solfrank.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <sys/cdefs.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "ext.h"
#include "fsutil.h"
#include "dosfs.h"
#include "fatcache.h"
#include "fragment.h"

#define SLOT_EMPTY      0x00        /* slot has never been used */
#define SLOT_E5         0x05        /* the real value is 0xe5 */
#define SLOT_DELETED    0xe5        /* file in this slot deleted */

#define ATTR_NORMAL     0x00        /* normal file */
#define ATTR_READONLY   0x01        /* file is readonly */
#define ATTR_HIDDEN     0x02        /* file is hidden */
#define ATTR_SYSTEM     0x04        /* file is a system file */
#define ATTR_VOLUME     0x08        /* entry is a volume label */
#define ATTR_DIRECTORY  0x10        /* entry is a directory name */
#define ATTR_ARCHIVE    0x20        /* file is new or modified */

#define ATTR_WIN95      0x0f        /* long name record */

/*
 * This is the format of the contents of the deTime field in the direntry
 * structure.
 * We don't use bitfields because we don't know how compilers for
 * arbitrary machines will lay them out.
 */
#define DT_2SECONDS_MASK    0x1F    /* seconds divided by 2 */
#define DT_2SECONDS_SHIFT   0
#define DT_MINUTES_MASK     0x7E0   /* minutes */
#define DT_MINUTES_SHIFT    5
#define DT_HOURS_MASK       0xF800  /* hours */
#define DT_HOURS_SHIFT      11

/*
 * This is the format of the contents of the deDate field in the direntry
 * structure.
 */
#define DD_DAY_MASK         0x1F    /* day of month */
#define DD_DAY_SHIFT        0
#define DD_MONTH_MASK       0x1E0   /* month */
#define DD_MONTH_SHIFT      5
#define DD_YEAR_MASK        0xFE00  /* year - 1980 */
#define DD_YEAR_SHIFT       9


/* dir.c */
static struct dosDirEntry *newDosDirEntry(void);
static void freeDosDirEntry(struct dosDirEntry *);
static struct dirTodoNode *newDirTodo(void);
static void freeDirTodo(struct dirTodoNode *);
static char *fullpath(struct dosDirEntry *);
static u_char calcShortSum(u_char *);
static int delete(int, struct bootblock *,struct cluster_chain_descriptor* ,cl_t, int,
    cl_t, int, int);
static int removede(int, struct bootblock *, struct cluster_chain_descriptor*,u_char *,
    u_char *, cl_t, cl_t, cl_t, char *, int);
static int checksize(struct bootblock *, u_char *,
    struct dosDirEntry *);
static int readDosDirSection(int, struct bootblock *,
    struct dosDirEntry *);

/*
 * Manage free dosDirEntry structures.
 */
static struct dosDirEntry *freede;

static struct dosDirEntry *
newDosDirEntry(void)
{
    struct dosDirEntry *de;

    if (!(de = freede)) {
        if (!(de = (struct dosDirEntry *)malloc(sizeof *de)))
            return 0;
    } else
        freede = de->next;
    return de;
}

static void
freeDosDirEntry(struct dosDirEntry *de)
{
    de->next = freede;
    freede = de;
}

/*
 * The same for dirTodoNode structures.
 */
static struct dirTodoNode *freedt;

static struct dirTodoNode *
newDirTodo(void)
{
    struct dirTodoNode *dt;

    if (!(dt = freedt)) {
        if (!(dt = (struct dirTodoNode *)malloc(sizeof *dt)))
            return 0;
    } else
        freedt = dt->next;
    return dt;
}

static void
freeDirTodo(struct dirTodoNode *dt)
{
    dt->next = freedt;
    freedt = dt;
}

/*
 * The stack of unread directories
 */
struct dirTodoNode *pendingDirectories = NULL;

/*
 * Return the full pathname for a directory entry.
 */
static char *
fullpath(struct dosDirEntry *dir)
{
    static char namebuf[MAXPATHLEN + 1];
    char *cp, *np;
    int nl;

    cp = namebuf + sizeof namebuf - 1;
    *cp = '\0';
    do {
        np = dir->lname[0] ? dir->lname : dir->name;
        nl = strlen(np);
        if ((cp -= nl) <= namebuf + 1)
            break;
        memcpy(cp, np, nl);
        *--cp = '/';
    } while ((dir = dir->parent) != NULL);
    if (dir)
        *--cp = '?';
    else
        cp++;
    return cp;
}

/*
 * Calculate a checksum over an 8.3 alias name
 */
static u_char
calcShortSum(u_char *p)
{
    u_char sum = 0;
    int i;

    for (i = 0; i < 11; i++) {
        sum = (sum << 7)|(sum >> 1);    /* rotate right */
        sum += p[i];
    }

    return sum;
}

/*
 * Global variables temporarily used during a directory scan
 */
static char longName[DOSLONGNAMELEN] = "";
static u_char *buffer = NULL;
static u_char *delbuf = NULL;

struct dosDirEntry *rootDir;
static struct dosDirEntry *lostDir;

/*
 * Init internal state for a new directory scan.
 */
int
resetDosDirSection(struct bootblock *boot)
{
    int b1, b2;
    int ret = FSOK;
    struct cluster_chain_descriptor *fat,tofind,*insert;
    struct fragment *frag,tofind_frag;
    struct fatcache *cache;
    b1 = boot->RootDirEnts * 32;
    b2 = boot->SecPerClust * boot->BytesPerSec;

    if (!(buffer = malloc(b1 > b2 ? b1 : b2))
        || !(delbuf = malloc(b2))
        || !(rootDir = newDosDirEntry())) {
        perror("No space for directory");
        return FSFATAL;
    }
    memset(rootDir, 0, sizeof *rootDir);
    if (boot->flags & FAT32) {
        if (boot->RootCl < CLUST_FIRST || boot->RootCl >= boot->NumClusters) {
            pfatal("Root directory starts with cluster out of range(%u)",
                   boot->RootCl);
            return FSFATAL;
        }
        // In fat12 and fat16, the boot->RootCl is zero,you can not find its chain
        tofind.head = boot->RootCl;
        fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
        /* we have check the cluster chain in readfatAndcheckfat */
        if (!fat) {
            pwarn("can not find Root direntory in cluster chain\n");
            if (ask(1, "Fix")) {
                tofind_frag.head = boot->RootCl;
                frag = RB_FIND(FSCK_MSDOS_FRAGMENT,&rb_free_root,&tofind_frag);
                if (!frag) {
                    fsck_err("can not find Root direntory in free chain\n");
                    ret = FSFATAL;
                    goto out;
                }
                /* find it in free rb tree,now move it to cluster chain */
                fat = New_fatentry();
                if (!fat)
                    return FSFATAL;
                fat->head = boot->RootCl;
                fat->head = 1;
                cache = New_fatcache();
                if (!cache)
                    return FSFATAL;
                cache->head = boot->RootCl;
                cache->length = 1;
                add_fatcache_To_ClusterChain(fat,cache);
                insert = RB_INSERT(FSCK_MSDOS_CACHE,&rb_root,fat);
                if (insert) {
                    fsck_err("%s:fatentry(head:0x%x) exist\n",__func__,fat->head);
                    return FSFATAL;
                }
                RB_REMOVE(FSCK_MSDOS_FRAGMENT,&rb_free_root,frag);
                free(frag);
                ret = FSFATMOD;
                goto out;
            } else {
                ret = FSFATAL;
                goto out;
            }
        }
        fat->flag |= FAT_USED;
        rootDir->head = boot->RootCl;
    }
out:
    return ret;
}

/*
 * Cleanup after a directory scan
 */
void
finishDosDirSection(void)
{
    struct dirTodoNode *p, *np;
    struct dosDirEntry *d, *nd;

    for (p = pendingDirectories; p; p = np) {
        np = p->next;
        freeDirTodo(p);
    }
    pendingDirectories = 0;
    for (d = rootDir; d; d = nd) {
        if ((nd = d->child) != NULL) {
            d->child = 0;
            continue;
        }
        if (!(nd = d->next))
            nd = d->parent;
        freeDosDirEntry(d);
    }
    rootDir = lostDir = NULL;
    free(buffer);
    free(delbuf);
    buffer = NULL;
    delbuf = NULL;
}

/*
 * Delete directory entries between startcl, startoff and endcl, endoff.
 */
static int
delete(int f, struct bootblock *boot, struct cluster_chain_descriptor *fat,cl_t startcl,
    int startoff, cl_t endcl, int endoff, int notlast)
{
    u_char *s, *e;
    loff_t off;
    int clsz = boot->SecPerClust * boot->BytesPerSec;
    struct fatcache *cache;
    fsck_debug("delete: %u:%u -->> %u:%u\n",startcl,startoff,endcl,endoff);
    s = delbuf + startoff;
    e = delbuf + clsz;
    if (!fat) {
        pwarn("Can't find cl %d in cluster chain\n",startcl);
        return FSFATAL;
    }
    while (startcl >= CLUST_FIRST && startcl < boot->NumClusters) {
        if (startcl == endcl) {
            if (notlast)
                break;
            e = delbuf + endoff;
        }
        off = startcl * boot->SecPerClust + boot->ClusterOffset;
        off *= boot->BytesPerSec;
        if (lseek64(f, off, SEEK_SET) != off) {
            perror("Unable to lseek64");
            return FSFATAL;
        }
        if (read(f, delbuf, clsz) != clsz) {
            perror("Unable to read directory");
            return FSFATAL;
        }
        while (s < e) {
            *s = SLOT_DELETED;
            s += 32;
        }
        if (lseek64(f, off, SEEK_SET) != off) {
            perror("Unable to lseek64");
            return FSFATAL;
        }
        if (write(f, delbuf, clsz) != clsz) {
            perror("Unable to write directory");
            return FSFATAL;
        }
        if (startcl == endcl)
            break;
        // if startcl = 0.it means no next cluster
        cache = Find_nextclus(fat, startcl, &startcl);
        if (!cache && !startcl)
            return FSFATAL;
        s = delbuf;
    }
    return FSOK;
}

static int
removede(int f, struct bootblock *boot,struct cluster_chain_descriptor *fat, u_char *start,
    u_char *end, cl_t startcl, cl_t endcl, cl_t curcl, char *path, int type)
{
    fsck_debug("removede : %u:%p --->> %u:%p \n",startcl,start,endcl,end);
    switch (type) {
    case 0:
        pwarn("Invalid long filename entry for %s\n", path);
        break;
    case 1:
        pwarn("Invalid long filename entry at end of directory %s\n", path);
        break;
    case 2:
        pwarn("Invalid long filename entry for volume label\n");
        break;
    case 3:
        pwarn("Can't find the associated cluster chain\n");
    }
    if (ask(1, "Remove")) {
        if (startcl != curcl) {
            if (delete(f, boot, fat,
                   startcl, start - buffer,
                   endcl, end - buffer,
                   endcl == curcl) == FSFATAL)
                return FSFATAL;
            start = buffer;
        }
        if (endcl == curcl)
            for (; start < end; start += 32)
                *start = SLOT_DELETED;
        return FSDIRMOD;
    }
    return FSERROR;
}

/*
 * Check an in-memory file entry
 */
static int
checksize(struct bootblock *boot, u_char *p,
    struct dosDirEntry *dir)
{
    /*
     * Check size on ordinary files
     */
    struct cluster_chain_descriptor *fat = NULL,tofind;
    struct fatcache *cache = NULL;
    u_int64_t physicalSize;
    const u_int64_t max_physical_size = 0x100000000;

    if (dir->head == CLUST_FREE)
        physicalSize = 0;
    else {
        if (dir->head < CLUST_FIRST || dir->head >= boot->NumClusters)
            return FSERROR;
        tofind.head = dir->head;
        fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
        if (!fat) {
            pwarn("Can't find the cluster chain with head(%u) \n",dir->head);
            return FSERROR;
        }
        physicalSize = ((u_int64_t)fat->length) * ((u_int64_t)boot->ClusterSize);
    }
    /*
     * The file size means how much useful data the file contains
     * FAT use 4 bytes to describe the file size, so the max file size is (4GBytes - 1).
     * The physical size means how much space the file use in disk. The max physical
     * size is 4GBytes.
     *
     * For example, we create a new file in WINDOWS,which size is 100Bytes, but i
     * use 4KB in disk. Here 100B is file size ,and 4KB is physical size.
     *
     * Physical size = (file size + boot->ClusterSize - 1) & (boot->ClusterSize - 1)
     */
    if (physicalSize > max_physical_size) {
        fsck_err("file %s physical size exceed 4 GBytes. %llu\n",
                fullpath(dir), physicalSize);
        return FSERROR;
    }

    if (physicalSize < (u_int64_t)dir->size) {
        pwarn("size of %s is %u, should at most be %llu\n",
              fullpath(dir), dir->size, physicalSize);
        fsck_debug("physicalSize:%llu ,dir->size = %d ,dir->head:0x%x\n",physicalSize,dir->size,dir->head);
        if (ask(1, "Truncate")) {
            dir->size = physicalSize;
            p[28] = (u_char)physicalSize;
            p[29] = (u_char)(physicalSize >> 8);
            p[30] = (u_char)(physicalSize >> 16);
            p[31] = (u_char)(physicalSize >> 24);
            return FSDIRMOD;
        } else
            return FSERROR;
    } else if (physicalSize - (u_int64_t)dir->size >= (u_int64_t)boot->ClusterSize) {
        pwarn("%s has too many clusters allocated\n",
              fullpath(dir));
        fsck_debug("physicalSize:%llu ,dir->size = %d ,dir->head:0x%x\n",physicalSize,dir->size,dir->head);
        if (ask(1, "Drop superfluous clusters")) {
            cl_t cl;
            u_int32_t sz = 0;

            for (cl = dir->head; (sz += boot->ClusterSize) < dir->size;) {
                cache = Find_nextclus(fat,cl,&cl);
                if (!cl )
                    return FSERROR;
                if (cl == CLUST_EOF)
                    break;
            }
            if (!cache && !cl)
                return FSERROR;

            Trunc(boot,fat,cl);
            fsck_debug("after truncate ,fat->length = %d \n",fat->length);
            return FSFATMOD;
        } else
            return FSERROR;
    }
    return FSOK;
}


static u_char  dot_header[16]={0x2E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00};
static u_char  dot_dot_header[16]={0x2E, 0x2E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00};

/*
 * Check for missing or broken '.' and '..' entries.
 */
static int
check_dot_dot(int f, struct bootblock *boot,struct dosDirEntry *dir)
{
    u_char *p, *buf;
    loff_t off;
    int last;
    cl_t cl;
    int rc=0, n_count;
    struct cluster_chain_descriptor *fat,tofind;
    struct fatcache *cache;
    int dot, dotdot;
    dot = dotdot = 0;
    cl = dir->head;
    if (dir->parent && (cl < CLUST_FIRST || cl >= boot->NumClusters)) {
        return rc;
    }
    tofind.head = dir->head;
    fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
    if (!fat) {
        pwarn("%s:cannot find cluster chain(%d)\n",__func__,dir->head);
        return FSFATAL;
    }
    do {
        if (!(boot->flags & FAT32) && !dir->parent) {
            last = boot->RootDirEnts * 32;
            off = boot->ResSectors + boot->FATs * boot->FATsecs;
        } else {
            last = boot->SecPerClust * boot->BytesPerSec;
            off = cl * boot->SecPerClust + boot->ClusterOffset;
        }

        off *= boot->BytesPerSec;
        buf = malloc(last);
        if (!buf) {
            perror("Unable to malloc");
            return FSFATAL;
        }
        if (lseek64(f, off, SEEK_SET) != off) {
            perror("Unable to lseek64");
            return FSFATAL;
        }
        if (read(f, buf, last) != last) {
            perror("Unable to read");
            return FSFATAL;
        }
        last /= 32;
        p = buf;
        for (n_count=0, rc=0; n_count < 11; n_count++) {
            if (dot_header[n_count] != p[n_count]) {
                rc=-1;
                break;
            }
        }
        if (!rc)
            dot=1;

        for (n_count = 0, rc = 0; n_count < 11; n_count++) {
            if (dot_dot_header[n_count] != p[n_count+32]) {
                rc=-1;
                break;
            }
        }
        if (!rc)
            dotdot=1;
        free(buf);
        cache = Find_nextclus(fat,cl,&cl);
        if (!cache && !cl)
            return FSFATAL;
    } while (cl >= CLUST_FIRST && cl < boot->NumClusters);

    if (!dot || !dotdot) {
        if (!dot)
            pwarn("%s: '.' absent for %s.\n",__func__,dir->name);

        if (!dotdot)
            pwarn("%s: '..' absent for %s. \n",__func__,dir->name);
        return -1;
    }
    return 0;
}

/*
 * Read a directory and
 *   - resolve long name records
 *   - enter file and directory records into the parent's list
 *   - push directories onto the todo-stack
 */
static int
readDosDirSection(int f, struct bootblock *boot,
    struct dosDirEntry *dir)
{
    struct dosDirEntry dirent, *d;
    u_char *p, *vallfn, *invlfn, *empty;
    loff_t off;
    int i, j, k, last;
    cl_t cl, valcl = ~0, invcl = ~0, empcl = ~0;
    char *t;
    u_int lidx = 0;
    int shortSum;
    int mod = FSOK;
    struct cluster_chain_descriptor *fat = NULL,tofind;
    struct fatcache *cache = NULL;
    #define THISMOD 0x8000          /* Only used within this routine */
    cl = dir->head;
    if (dir->parent && (cl < CLUST_FIRST || cl >= boot->NumClusters)) {
        /*
         * Already handled somewhere else.
         */
        return FSOK;
    }
    shortSum = -1;
    vallfn = invlfn = empty = NULL;
    int dot = 0, dotdot = 0;
    dot    = dot;
    dotdot = dotdot;
    // FAT32 has boot->Rootcl
    if ((boot->ClustMask == CLUST32_MASK) || dir->parent) {
        tofind.head = dir->head;
        fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
        if (!fat) {
            fsck_err("%s:can not find cluster chain(head = %u)\n",__func__,dir->head);
            return FSFATMOD;
        }
    }
    do {
        struct cluster_chain_descriptor *c_fat;
        if (!(boot->flags & FAT32) && !dir->parent) {
            last = boot->RootDirEnts * 32;
            off = boot->ResSectors + boot->FATs * boot->FATsecs;
        } else {
            last = boot->SecPerClust * boot->BytesPerSec;
            off = cl * boot->SecPerClust + boot->ClusterOffset;
        }

        off *= boot->BytesPerSec;
        if (lseek64(f, off, SEEK_SET) != off) {
            printf("off = %llu\n", off);
            perror("Unable to lseek64");
            return FSFATAL;
        }
        if (read(f, buffer, last) != last) {
            perror("Unable to read");
            return FSFATAL;
        }
        last /= 32;
        for (p = buffer, i = 0; i < last; i++, p += 32) {
            if (dir->fsckflags & DIREMPWARN) {
                *p = SLOT_EMPTY;
                continue;
            }

            if (*p == SLOT_EMPTY || *p == SLOT_DELETED) {
                if (*p == SLOT_EMPTY) {
                    dir->fsckflags |= DIREMPTY;
                    empty = p;
                    empcl = cl;
                }
                continue;
            }

            if (dir->fsckflags & DIREMPTY) {
                if (!(dir->fsckflags & DIREMPWARN)) {
                    pwarn("%s has entries after end of directory\n",
                          fullpath(dir));
                    if (ask(1, "Extend")) {
                        u_char *q;

                        dir->fsckflags &= ~DIREMPTY;
                        if (delete(f, boot, fat,
                               empcl, empty - buffer,
                               cl, p - buffer, 1) == FSFATAL)
                            return FSFATAL;
                        q = empcl == cl ? empty : buffer;
                        for (; q < p; q += 32)
                            *q = SLOT_DELETED;
                        mod |= THISMOD|FSDIRMOD;
                    } else if (ask(1, "Truncate"))
                        dir->fsckflags |= DIREMPWARN;
                }
                if (dir->fsckflags & DIREMPWARN) {
                    *p = SLOT_DELETED;
                    mod |= THISMOD|FSDIRMOD;
                    continue;
                } else if (dir->fsckflags & DIREMPTY)
                    mod |= FSERROR;
                empty = NULL;
            }

            if (p[11] == ATTR_WIN95) {
                if (*p & LRFIRST) {
                    if (shortSum != -1) {
                        if (!invlfn) {
                            invlfn = vallfn;
                            invcl = valcl;
                        }
                    }
                    memset(longName, 0, sizeof longName);
                    shortSum = p[13];
                    vallfn = p;
                    valcl = cl;
                } else if (shortSum != p[13]
                       || lidx != (*p & LRNOMASK)) {
                    if (!invlfn) {
                        invlfn = vallfn;
                        invcl = valcl;
                    }
                    if (!invlfn) {
                        invlfn = p;
                        invcl = cl;
                    }
                    vallfn = NULL;
                }
                lidx = *p & LRNOMASK;
                t = longName + --lidx * 13;
                for (k = 1; k < 11 && t < longName + sizeof(longName); k += 2) {
                    if (!p[k] && !p[k + 1])
                        break;
                    *t++ = p[k];
                    /*
                     * Warn about those unusable chars in msdosfs here? XXX
                     */
                    if (p[k + 1])
                        t[-1] = '?';
                }
                if (k >= 11)
                    for (k = 14; k < 26 && t < longName + sizeof(longName); k += 2) {
                        if (!p[k] && !p[k + 1])
                            break;
                        *t++ = p[k];
                        if (p[k + 1])
                            t[-1] = '?';
                    }
                if (k >= 26)
                    for (k = 28; k < 32 && t < longName + sizeof(longName); k += 2) {
                        if (!p[k] && !p[k + 1])
                            break;
                        *t++ = p[k];
                        if (p[k + 1])
                            t[-1] = '?';
                    }
                if (t >= longName + sizeof(longName)) {
                    pwarn("long filename too long\n");
                    if (!invlfn) {
                        invlfn = vallfn;
                        invcl = valcl;
                    }
                    vallfn = NULL;
                }
                if (p[26] | (p[27] << 8)) {
                    pwarn("long filename record cluster start != 0\n");
                    if (!invlfn) {
                        invlfn = vallfn;
                        invcl = cl;
                    }
                    vallfn = NULL;
                }
                continue;   /* long records don't carry further
                             * information */
            }

            /*
             * This is a standard msdosfs directory entry.
             */
            memset(&dirent, 0, sizeof dirent);

            /*
             * it's a short name record, but we need to know
             * more, so get the flags first.
             */
            dirent.flags = p[11];

            /*
             * Translate from 850 to ISO here       XXX
             */
            for (j = 0; j < 8; j++)
                dirent.name[j] = p[j];
            dirent.name[8] = '\0';
            for (k = 7; k >= 0 && dirent.name[k] == ' '; k--)
                dirent.name[k] = '\0';
            if (dirent.name[k] != '\0')
                k++;
            if (dirent.name[0] == SLOT_E5)
                dirent.name[0] = 0xe5;

            if (dirent.flags & ATTR_VOLUME) {
                if (vallfn || invlfn) {
                    mod |= removede(f, boot, fat,
                            invlfn ? invlfn : vallfn, p,
                            invlfn ? invcl : valcl, -1, 0,
                            fullpath(dir), 2);
                    vallfn = NULL;
                    invlfn = NULL;
                }
                continue;
            }

            if (p[8] != ' ')
                dirent.name[k++] = '.';
            for (j = 0; j < 3; j++)
                dirent.name[k++] = p[j+8];
            dirent.name[k] = '\0';
            for (k--; k >= 0 && dirent.name[k] == ' '; k--)
                dirent.name[k] = '\0';

            if (vallfn && shortSum != calcShortSum(p)) {
                if (!invlfn) {
                    invlfn = vallfn;
                    invcl = valcl;
                }
                vallfn = NULL;
            }
            dirent.head = p[26] | (p[27] << 8);
            if (boot->ClustMask == CLUST32_MASK)
                dirent.head |= (p[20] << 16) | (p[21] << 24);
            dirent.size = p[28] | (p[29] << 8) | (p[30] << 16) | (p[31] << 24);
            if (vallfn) {
                strcpy(dirent.lname, longName);
                longName[0] = '\0';
                shortSum = -1;
            }

            dirent.parent = dir;
            dirent.next = dir->child;

            if (invlfn) {
                mod |= k = removede(f, boot, fat,
                            invlfn, vallfn ? vallfn : p,
                            invcl, vallfn ? valcl : cl, cl,
                            fullpath(&dirent), 0);
                if (mod & FSFATAL)
                    return FSFATAL;
                if (vallfn
                    ? (valcl == cl && vallfn != buffer)
                    : p != buffer)
                    if (k & FSDIRMOD)
                        mod |= THISMOD;
            }

            if (dirent.size == 0 && !(dirent.flags & ATTR_DIRECTORY)) {
                if (dirent.head != 0) {
                    pwarn("%s has clusters, but size 0\n",
                          fullpath(&dirent));
                    if (ask(1, "Drop allocated clusters")) {
                        p[26] = p[27] = 0;
                        if (boot->ClustMask == CLUST32_MASK)
                            p[20] = p[21] = 0;
                        tofind.head = dirent.head;
                        c_fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
                        if (c_fat)
                            clearchain(boot,c_fat, dirent.head);
                        else
                            mod |= FSERROR;
                        dirent.head = 0;
                        mod |= THISMOD|FSDIRMOD|FSFATMOD;
                    } else
                        mod |= FSERROR;
                }
            } else if (dirent.head == 0
                   && !strcmp(dirent.name, "..")
                   && dir->parent           /* XXX */
                   && !dir->parent->parent) {
                /*
                 *  Do nothing, the parent is the root
                 */
            } else if (dirent.head < CLUST_FIRST
                   || dirent.head >= boot->NumClusters) {
                if (dirent.head == 0)
                    pwarn("%s has no clusters\n",
                          fullpath(&dirent));
                else if (dirent.head < CLUST_FIRST
                     || dirent.head >= boot->NumClusters)
                    pwarn("%s starts with cluster out of range(%u)\n",
                          fullpath(&dirent),
                          dirent.head);

                if (dirent.flags & ATTR_DIRECTORY) {
                    if (ask(1, "Remove")) {
                        *p = SLOT_DELETED;
                        mod |= THISMOD|FSDIRMOD;
                    } else
                        mod |= FSERROR;
                    continue;
                } else {
                    if (ask(1, "Truncate")) {
                        p[28] = p[29] = p[30] = p[31] = 0;
                        p[26] = p[27] = 0;
                        if (boot->ClustMask == CLUST32_MASK)
                            p[20] = p[21] = 0;
                        dirent.size = 0;
                        mod |= THISMOD|FSDIRMOD;
                    } else
                        mod |= FSERROR;
                }
            }

            if (dirent.head >= CLUST_FIRST && dirent.head < boot->NumClusters) {
                tofind.head = dirent.head;
                c_fat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
                if (c_fat) {
                    c_fat->flag |= FAT_USED;
                } else {
                    fsck_warn("can't find cluster chain(head:0x%x) of file:%s \n",dirent.head,fullpath(&dirent));
                    if (vallfn || invlfn) {
                        fsck_warn("Invalid long directory\n");
                        mod |= removede(f, boot, fat,invlfn ? invlfn : vallfn, p,invlfn ? invcl : valcl, -1, 0,fullpath(dir), 3);
                    } else {
                        fsck_warn("Invalid short directory\n");
                        if (ask(1, "Delete")) {
                            *p = SLOT_DELETED;
                            mod |= THISMOD|FSDIRMOD;
                        }else
                            mod |= FSERROR;
                    }
                }
            }
            vallfn = NULL; /* not used any longer */
            invlfn = NULL;

            if (dirent.flags & ATTR_DIRECTORY) {
                /*
                 * gather more info for directories
                 */
                struct dirTodoNode *n;

                if (dirent.size) {
                    pwarn("Directory %s has size != 0\n",
                          fullpath(&dirent));
                    if (ask(1, "Correct")) {
                        p[28] = p[29] = p[30] = p[31] = 0;
                        dirent.size = 0;
                        mod |= THISMOD|FSDIRMOD;
                    } else
                        mod |= FSERROR;
                }
                /*
                 * handle '.' and '..' specially
                 */
                if (strcmp(dirent.name, ".") == 0) {
                    if (dirent.head != dir->head) {
                        pwarn("'.' entry in %s has incorrect start cluster\n",
                              fullpath(dir));
                        if (ask(1, "Correct")) {
                            dirent.head = dir->head;
                            p[26] = (u_char)dirent.head;
                            p[27] = (u_char)(dirent.head >> 8);
                            if (boot->ClustMask == CLUST32_MASK) {
                                p[20] = (u_char)(dirent.head >> 16);
                                p[21] = (u_char)(dirent.head >> 24);
                            }
                            mod |= THISMOD|FSDIRMOD;
                        } else
                            mod |= FSERROR;
                    }
                    continue;
                } else if (strcmp(dirent.name, "..") == 0) {
                    if (dir->parent) {      /* XXX */
                        if (!dir->parent->parent) {
                            if (dirent.head) {
                                pwarn("'..' entry in %s has non-zero start cluster\n",
                                      fullpath(dir));
                                if (ask(1, "Correct")) {
                                    dirent.head = 0;
                                    p[26] = p[27] = 0;
                                    if (boot->ClustMask == CLUST32_MASK)
                                        p[20] = p[21] = 0;
                                    mod |= THISMOD|FSDIRMOD;
                                } else
                                    mod |= FSERROR;
                            }
                        } else if (dirent.head != dir->parent->head) {
                            pwarn("'..' entry in %s has incorrect start cluster\n",
                                  fullpath(dir));
                            if (ask(1, "Correct")) {
                                dirent.head = dir->parent->head;
                                p[26] = (u_char)dirent.head;
                                p[27] = (u_char)(dirent.head >> 8);
                                if (boot->ClustMask == CLUST32_MASK) {
                                    p[20] = (u_char)(dirent.head >> 16);
                                    p[21] = (u_char)(dirent.head >> 24);
                                }
                                mod |= THISMOD|FSDIRMOD;
                            } else
                                mod |= FSERROR;
                        }
                    }
                    continue;
                } else { //only one directory entry can point to dir->head, it's  '.'
                    if (dirent.head == dir->head) {
                        pwarn("%s entry in %s has incorrect start cluster.remove\n",
                                dirent.name, fullpath(dir));
                        //we have to remove this directory entry rigth now rigth here
                        if (ask(1, "Remove")) {
                            *p = SLOT_DELETED;
                            mod |= THISMOD|FSDIRMOD;
                        } else
                            mod |= FSERROR;
                        continue;
                    }
                    /* Consistency checking. a directory must have at least two entries:
                       a dot (.) entry that points to itself, and a dot-dot (..)
                       entry that points to its parent.
                     */
                    if (check_dot_dot(f,boot,&dirent)) {
                        //mark directory entry as deleted.
                        if (ask(1, "Remove")) {
                            *p = SLOT_DELETED;
                            mod |= THISMOD|FSDIRMOD;
                        } else
                            mod |= FSERROR;
                        continue;
                    }
                }

                /* create directory tree node */
                if (!(d = newDosDirEntry())) {
                    perror("No space for directory");
                    return FSFATAL;
                }
                memcpy(d, &dirent, sizeof(struct dosDirEntry));
                /* link it into the tree */
                dir->child = d;
                /* Enter this directory into the todo list */
                if (!(n = newDirTodo())) {
                    perror("No space for todo list");
                    return FSFATAL;
                }
                n->next = pendingDirectories;
                n->dir = d;
                pendingDirectories = n;
            } else {
                mod |= k = checksize(boot, p, &dirent);
                if (k & FSDIRMOD)
                    mod |= THISMOD;
            }
            boot->NumFiles++;
        }
        if (mod & THISMOD) {
            last *= 32;
            if (lseek64(f, off, SEEK_SET) != off
                || write(f, buffer, last) != last) {
                perror("Unable to write directory");
                return FSFATAL;
            }
            mod &= ~THISMOD;
        }
        if (boot->ClustMask == CLUST32_MASK || dir->parent) {
            cache = Find_nextclus(fat,cl,&cl);
            if (!cache && !cl) {
                fsck_err("%s :Find nextclus error \n",__func__);
                return FSFATAL;
            }
        }
    } while (cl  >= CLUST_FIRST && cl < boot->NumClusters);
    if (invlfn || vallfn)
        mod |= removede(f, boot, fat,
                invlfn ? invlfn : vallfn, p,
                invlfn ? invcl : valcl, -1, 0,
                fullpath(dir), 1);
    return mod & ~THISMOD;
}

int
handleDirTree(int dosfs, struct bootblock *boot)
{
    int mod;
    fsck_debug("rootDir->head :%d\n", rootDir->head);
    mod = readDosDirSection(dosfs, boot, rootDir);
    if (mod & FSFATAL)
        return FSFATAL;
    /*
     * process the directory todo list
     */
    while (pendingDirectories) {
        struct dosDirEntry *dir = pendingDirectories->dir;
        struct dirTodoNode *n = pendingDirectories->next;

        /*
         * remove TODO entry now, the list might change during
         * directory reads
         */
        freeDirTodo(pendingDirectories);
        pendingDirectories = n;

        /*
         * handle subdirectory
         */
        mod |= readDosDirSection(dosfs, boot, dir);
        if (mod & FSFATAL)
            return FSFATAL;
    }
    return mod;
}

/*
 * Try to reconnect a FAT chain into dir
 */
static u_char *lfbuf;
static cl_t lfcl;
static loff_t lfoff;
static struct cluster_chain_descriptor *lostdirfat;
int
reconnect(int dosfs, struct bootblock *boot, struct cluster_chain_descriptor *fat, cl_t head)
{
    struct dosDirEntry d;
    struct fatcache *cache = NULL;
    struct cluster_chain_descriptor tofind;
    u_char *p;

    cache = cache;
    if (!ask(1, "Reconnect"))
        return FSERROR;
    // find the lost dir in root directory
    if (!lostDir) {
        for (lostDir = rootDir->child; lostDir; lostDir = lostDir->next) {
            if (!strcmp(lostDir->name, LOSTDIR)) {
                tofind.head = lostDir->head;
                lostdirfat = RB_FIND(FSCK_MSDOS_CACHE,&rb_root,&tofind);
                break;
            }
        }
        if (!lostDir) {     /* Create LOSTDIR?      XXX */
            pwarn("No %s directory\n", LOSTDIR);
            return FSERROR;
        }
    }
    if (!lfbuf) {
        lfbuf = malloc(boot->ClusterSize);
        if (!lfbuf) {
            perror("No space for buffer");
            return FSFATAL;
        }
        p = NULL;
    } else
        p = lfbuf;
    while (1) {
        if (p)
            // find an empty or deleted direntry in lostdir
            for (; p < lfbuf + boot->ClusterSize; p += 32)
                if (*p == SLOT_EMPTY
                    || *p == SLOT_DELETED)
                    break;
        if (p && p < lfbuf + boot->ClusterSize)
            break;
        if (p) {
            cache = Find_nextclus(lostdirfat,lfcl,&lfcl);
        } else {
            lfcl = lostDir->head;
        }
    //  lfcl = p ? fat[lfcl].next : lostDir->head;
        if (lfcl < CLUST_FIRST || lfcl >= boot->NumClusters) {
            /* Extend LOSTDIR?              XXX */
            pwarn("No space in %s\n", LOSTDIR);
            lfcl = (lostDir->head < boot->NumClusters) ? lostDir->head : 0;
            return FSERROR;
        }
        lfoff = lfcl * boot->ClusterSize
            + boot->ClusterOffset * boot->BytesPerSec;
        if (lseek64(dosfs, lfoff, SEEK_SET) != lfoff
            || read(dosfs, lfbuf, boot->ClusterSize) != boot->ClusterSize) {
            perror("could not read LOST.DIR");
            return FSFATAL;
        }
        p = lfbuf;
    }

    boot->NumFiles++;
    /* Ensure uniqueness of entry here!             XXX */
    memset(&d, 0, sizeof d);
    (void)snprintf(d.name, sizeof(d.name), "%u", head);
    d.flags = 0;
    d.head = head;
    d.size = fat->length * boot->ClusterSize;

    memset(p, 0, 32);
    memset(p, ' ', 11);
    memcpy(p, d.name, strlen(d.name));
    p[26] = (u_char)d.head;
    p[27] = (u_char)(d.head >> 8);
    if (boot->ClustMask == CLUST32_MASK) {
        p[20] = (u_char)(d.head >> 16);
        p[21] = (u_char)(d.head >> 24);
    }
    p[28] = (u_char)d.size;
    p[29] = (u_char)(d.size >> 8);
    p[30] = (u_char)(d.size >> 16);
    p[31] = (u_char)(d.size >> 24);
    fat->flag |= FAT_USED;
    if (lseek64(dosfs, lfoff, SEEK_SET) != lfoff
        || write(dosfs, lfbuf, boot->ClusterSize) != boot->ClusterSize) {
        perror("could not write LOST.DIR");
        return FSFATAL;
    }
    return FSDIRMOD;
}

void
finishlf(void)
{
    if (lfbuf)
        free(lfbuf);
    lfbuf = NULL;
}