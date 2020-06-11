#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
//#include <strings.h>

/*
 * How to update NOR flash layout:
 * 1. Modify mxp_gen.c for change layout
 * 2. Build: gcc mxp_gen.c -o mxp_gen
 * 3. Generate MXP_SF.bin: ./mxp_gen MXP_SF.bin
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define MAX_RECORD_PRINT_COUNT 32

typedef struct {

    u8    	magic_prefix[4];//MXPT
    u8      version;
    u8  	type; //0X00:tag 0xFF:terminated 0x01:general
    u8  	format;
    u8    padding;
    u64		start;
    u64		size;
    u32		block;
    u32		block_count;
    u8	    name[16]; //should be 0 terminated
    u8		backup[16]; //should be 0 terminated
    u8		hash[32];
    u8		reserved[23];
    u32		crc32;
    u8		status;
    u8 		magic_suffix[4];//TPXM

} mxp_record;


#define MXP_PART_MAGIC_PREFIX			"MXPT"
#define MXP_PART_MAGIC_SUFFIX			"TPXM"

#define MXP_STORAGE_TYPE_FLASH       	0x01

#define MXP_PART_TYPE_TAG 				0x00
#define MXP_PART_TYPE_NORMAL			0x01
#define MXP_PART_TYPE_MTD 			    0x02

#define MXP_PART_FORMAT_NONE 			0x00


#define MXP_PART_STATUS_UPGRADING		0xF7
#define MXP_PART_STATUS_READY			0xF6
#define MXP_PART_STATUS_ACTIVE			0xF5
#define MXP_PART_STATUS_INACTIVE		0xF4
#define MXP_PART_STATUS_EMPTY			0x00

#define TABLE_SIZE 4096
#define COUNT (TABLE_SIZE/sizeof(mxp_record))

mxp_record mxpt[COUNT];
void out_record(const char* name, u8 type, u32 start, u32 size, u8 status,u8 index)
{

    strcpy(mxpt[index].name,name);
    mxpt[index].type=type;
    mxpt[index].start=start;
    mxpt[index].size=size;
    mxpt[index].status=status;
    mxpt[index].version=0x01;
}

static void print_mxp_record(int index,mxp_record* rc)
{
    printf("[mxp_record]: %d\n",index);
    printf("     name: %s\n",rc->name);
    printf("     type: 0x%02X\n",rc->type);
    printf("   format: 0x%02X\n",rc->format);
    printf("   backup: %s\n",( ((char)0xFF)==((char)rc->backup[0]) )?(""):((char *)rc->backup));
    printf("    start: 0x%08X\n",(unsigned int)rc->start);
    printf("     size: 0x%08X\n",(unsigned int)rc->size);
    printf("   status: 0x%02X\n",rc->status);
    printf("\n");

}

int main(int argc, char *argv[])
{

    ssize_t nrd;
    int fd;

    int i=0;
    printf(":[%d]\n",__LINE__);
    memset(mxpt,0xFF,TABLE_SIZE);

    if(0==strcmp(argv[1],"-i"))
    {
        int i=0;
        fd = open(argv[2], O_RDONLY);
        read(fd,mxpt,TABLE_SIZE);
        for(i=0;i<MAX_RECORD_PRINT_COUNT;i++)
        {
            if(MXP_PART_TYPE_TAG==mxpt[i].type)
            {
                break;
            }
            print_mxp_record(i,&mxpt[i]);
        }
        printf("Available MXP record count:%d\n",i);
        close(fd);
        return 0;
    }

    fd = open(argv[1], O_CREAT | O_RDWR);

    //initiailize table
    for(i=0;i<COUNT;i++)
    {
        memcpy(mxpt[i].magic_prefix,"MXPT",4);
        memcpy(mxpt[i].magic_suffix,"TPXM",4);
        mxpt[i].type=MXP_PART_TYPE_TAG;
        memset(mxpt[i].name,0x00,16);
        memset(mxpt[i].backup,0x00,16);
        mxpt[i].format=MXP_PART_FORMAT_NONE;
        mxpt[i].status=MXP_PART_STATUS_EMPTY;
        mxpt[i].version=0x01;
    }

    i=0;
    out_record("IPL",       MXP_PART_TYPE_NORMAL,                       0x00000000, 0x00010000, MXP_PART_STATUS_EMPTY, i++); //don't modify
    out_record("IPL_CUST",  MXP_PART_TYPE_NORMAL,                       0x00010000, 0x0000F000, MXP_PART_STATUS_EMPTY, i++); //don't modify
    out_record("KEY_CUST",  MXP_PART_TYPE_NORMAL,                       0x0001F000, 0x00001000, MXP_PART_STATUS_EMPTY, i++);
    out_record("MXPT",      MXP_PART_TYPE_NORMAL,                       0x00020000, 0x00001000, MXP_PART_STATUS_EMPTY, i++); //0x3000,0x20000,0x40000
    out_record("UBOOT",     MXP_PART_TYPE_NORMAL,                       0x00030000, 0x0001F000, MXP_PART_STATUS_EMPTY, i++);
    out_record("UBOOT_ENV", MXP_PART_TYPE_NORMAL,                       0x0004F000, 0x00001000, MXP_PART_STATUS_EMPTY, i++);
    out_record("BOOT",      (MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00000000, 0x00050000, MXP_PART_STATUS_EMPTY, i++);
    out_record("KERNEL",    (MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00050000, 0x00200000, MXP_PART_STATUS_EMPTY, i++);
    out_record("rootfs",    (MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00250000, 0x00400000, MXP_PART_STATUS_EMPTY, i++);
    out_record("nvrservice",(MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00650000, 0x00300000, MXP_PART_STATUS_EMPTY, i++);
	out_record("customer",  (MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00950000, 0x006A0000, MXP_PART_STATUS_EMPTY, i++);
	out_record("private",   (MXP_PART_TYPE_NORMAL|MXP_PART_TYPE_MTD),   0x00FF0000, 0x00010000, MXP_PART_STATUS_EMPTY, i++);

    write(fd,mxpt,TABLE_SIZE);
    close(fd);

    //dump
    for(i=0;i<MAX_RECORD_PRINT_COUNT;i++)
    {
        if(MXP_PART_TYPE_TAG==mxpt[i].type)
        {
        break;
        }

        printf("%12s:",mxpt[i].name);
        if((unsigned int)mxpt[i].size != 0xFFFFFFFF)
        {
            printf(" 0x%08X-0x%08X",(unsigned int)mxpt[i].start, (unsigned int)mxpt[i].start+(unsigned int)mxpt[i].size);
            printf(" size:%dKB",(unsigned int)mxpt[i].size/1024);
        }
        else
        {
            printf(" 0x%08X-0x%08X",(unsigned int)mxpt[i].start, 0xFFFFFFFF);
            printf(" size:-%dKB",(unsigned int)mxpt[i].start/1024);
        }
        printf("\n");

    }
    printf("Available MXP record count:%d\n",i);


    return 0;
}
