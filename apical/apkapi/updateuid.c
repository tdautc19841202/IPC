#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include "apkapi.h"

#define SID_LEN   32
#define UUID_LEN  20
#define DEVID_LEN 52

static int read_line(int fd, char *buf, int size)
{
    uint8_t byte = 0;
    int     ret;
    int     i;

    // skip spaces
    for (i=0; i<size; i++) {
        ret = read(fd, &byte, 1);
        if (ret != 1 || (byte != ' ' && byte != '\r')) {
            break;
        }
    }

    // if empty line
    if (ret != 1 || byte == '\n') {
        buf[0] = '\0';
        return 0;
    }

    // start read line
    buf[0] = byte;
    for (i=1; i<size; i++) {
        ret = read(fd, buf + i, 1);
        if (ret != 1 || buf[i] == ' ' || buf[i] == '\r' || buf[i] == '\n') {
            break;
        }
    }

    // read until line end
    if (buf[i] != '\n') {
        while (1) {
            ret = read(fd, &byte, 1);
            if (ret != 1 || byte == '\n') {
                break;
            }
        }
    }

    if (buf[i] == ' ' || buf[i] == '\r' || buf[i] == '\n' || i == size - 1) {
        buf[i] = '\0';
        return i;
    }

    buf[i+1] = '\0';
    return i + 1;
}

int get_file_size(char *file_path)
{
      FILE * fp;
      long size;

      fp = fopen (file_path,"rb");
      if (fp==NULL) {
          printf("Error opening file");
          return 0;
      } else {
          fseek (fp, 0, SEEK_END);
          size=ftell(fp);
          fclose (fp);
          printf ("Size of %s: %ld bytes.\n", file_path, size);
          return size;
      }  
}

int sdcard_update_uid(char *path)
{
    FILE *devid_file;
    FILE *record_file;
    FILE *burned_file;
    char uuid_txt  [PATH_MAX];
    char recid_log [PATH_MAX];
    char burned_log[PATH_MAX];
    char line_num  [5]            = {0};
    char sid_cur   [SID_LEN   +1] = {0};
    char uuid_cur  [UUID_LEN  +1] = {0};
    char devid     [DEVID_LEN +1] = {0};
    int  fd                       = 0;
    int  uuid_file_size           = 0;
    
    strcpy(uuid_txt  , path); strcat(uuid_txt  , "/uuid/UUIDList.txt");
    strcpy(recid_log , path); strcat(recid_log , "/uuid/RecordID.log");
    strcpy(burned_log, path); strcat(burned_log, "/uuid/BurnedID.log");

    uuid_file_size = get_file_size(uuid_txt);
    fd = open(recid_log, O_RDONLY);
    if (fd < 0) {
        printf("failed to open RecordID.log file !\n");
        return -1;
    }
    read_line(fd, line_num, 5);
    close(fd);
    
    if (atoi(line_num) > uuid_file_size/(DEVID_LEN + 2)) {
        return -2;
    }
    
    if((devid_file = fopen(uuid_txt,"r")) == NULL) {
        printf("failed to open UUIDList.txt file !\n");
        return -1;
    }
    
    fseek(devid_file,(DEVID_LEN+2)*atoi(line_num),SEEK_SET);
    fread(devid, DEVID_LEN, 1, devid_file);
    devid[DEVID_LEN] = '\0';
    printf("devid = %s\n",devid);
    fclose(devid_file);
    
    memcpy(uuid_cur, devid, UUID_LEN);
    uuid_cur[UUID_LEN] = '\0';
    memcpy(sid_cur, devid+UUID_LEN, SID_LEN);
    sid_cur[SID_LEN] = '\0';
    printf("uuid = %s\n",uuid_cur);
    printf("sid = %s\n",sid_cur);
    
    if((record_file = fopen(recid_log,"w")) == NULL) {
        printf("failed to write RecordID.log file\n");
        return -1;
    }
    fprintf(record_file,"%d\n",atoi(line_num)+1);
    fclose(record_file);
    
    if (set_dev_ids(NULL,NULL,sid_cur,uuid_cur) != -1) {
        if((burned_file = fopen(burned_log,"a+")) != NULL) {
            fwrite(devid, DEVID_LEN, 1, burned_file);
            fwrite("\r\n", 2, 1, burned_file);
        } else {
            printf("failed to write BurnedID.txt file\n");
            return -1;
        }
        fclose(burned_file);
    } else {
        printf("failed to write uuid !\n");
        return -1;
    }
    return 0;
}


