/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_ipc_sd_demo
**********************************************************************************/
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "tuya_ipc_api.h"
#include "tuya_ipc_common.h"
#include "tuya_ipc_dp_handler.h"
#include "tuya_ipc_stream_storage.h"
#include "apkapi.h"

/************************
Description: Using the TUYA SD card storage and playback function,
the developers need to implement the relevant interface.
Note：Interface names cannot be changed, and these interfaces are declared and used in SDK.
This demo file gives the management implementation of SD card operation for typical embedded devices in Linux system.
Developers can modify it according to their practical application.

VOID tuya_ipc_sd_format(VOID);
VOID tuya_ipc_sd_remount(VOID);
E_SD_STATUS tuya_ipc_sd_get_status(VOID);
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free);
CHAR_T *tuya_ipc_get_sd_mount_path(VOID);

************************/
#define SD_MOUNT_PATH  "/tmp/sdcard"

#define TYDEBUG printf
#define TYERROR printf

//Implementation Interface of Formatting Operation
VOID tuya_ipc_sd_format(VOID)
{
    uint32_t status;
    TYDEBUG("sd format begin\n");
    send_msg_to_ipcam("format", NULL);
    while (1) {
        get_sdcard_status_from_shm(&status, NULL, NULL);
        if (!(status & (SDSTATUS_REQ_FORMAT|SDSTATUS_FORMATTING))) break;
        usleep(100*1000);
    }
    TYDEBUG("sd format end\n");
}

//Implementation Interface for Remounting
VOID tuya_ipc_sd_remount(VOID)
{
    char buffer[512] = {0};
    E_SD_STATUS status = SD_STATUS_UNKNOWN;

    status = tuya_ipc_sd_get_status();
    if (SD_STATUS_NORMAL == status) {
        TYDEBUG("sd don't need to remount!\n");
        return;
    }

    TYDEBUG("remount_sd_card ..... \n");
    FILE *pp = popen("mount -o remount "SD_MOUNT_PATH, "r");
    if (NULL != pp) {
        fgets(buffer, sizeof(buffer), pp);
        printf("%s\n", buffer);
        pclose(pp);
    } else {
        printf("remount_sd_card failed\n");
    }
}

STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID)
{
    BOOL_T sd_on = IPC_APP_get_sd_record_onoff();
    if (sd_on) {
        UINT_T sd_mode = IPC_APP_get_sd_record_mode();
        if (0 == sd_mode) {
            return SS_WRITE_MODE_EVENT;
        } else {
            return SS_WRITE_MODE_ALL;
        }
    } else {
        return SS_WRITE_MODE_NONE;
    }
}

//Implementation Interface for Obtaining SD Card Status
E_SD_STATUS tuya_ipc_sd_get_status(VOID)
{
    int64_t  t = 0, f = 0;
    uint32_t s;
    get_sdcard_status_from_shm(&s, &t, &f);
    if (!(s & SDSTATUS_INSERTED)) return SD_STATUS_NOT_EXIST;
    else if (s & SDSTATUS_FORMATTING) return SD_STATUS_FORMATING;
    else if (f > 0) return SD_STATUS_NORMAL;
    else return SD_STATUS_ABNORMAL;
}

//SD card capacity acquisition interface, unit: KB
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free)
{
    int64_t t = 0, f = 0;
    get_sdcard_status_from_shm(NULL, &t, &f);
    *p_total = t / 1024;
    *p_used  = (t - f) / 1024;
    *p_free  = f / 1024;
    printf("curr sd total:%u used:%u p_free:%u\r\n", *p_total, *p_used, *p_free);
}

//get the path of mounting sdcard
CHAR_T *tuya_ipc_get_sd_mount_path(VOID)
{
    return SD_MOUNT_PATH;
}

//The maximum number of events per day, exceeding this value, there will be an exception when playback and can not play.
//Too much setting of this value will affect the query efficiency
#define MAX_EVENT_NUM_PER_DAY   (500)
extern IPC_MEDIA_INFO_S s_media_info;

OPERATE_RET TUYA_APP_Init_Stream_Storage(IN CONST CHAR_T *p_sd_base_path)
{
    STATIC BOOL_T s_stream_storage_inited = FALSE;
    if (s_stream_storage_inited == TRUE) {
        PR_DEBUG("The Stream Storage Is Already Inited");
        return OPRT_OK;
    }

    PR_DEBUG("Init Stream_Storage SD:%s", p_sd_base_path);
    OPERATE_RET ret = tuya_ipc_ss_init((CHAR_T*)p_sd_base_path, &s_media_info, MAX_EVENT_NUM_PER_DAY,NULL);
    if (ret != OPRT_OK) {
        PR_ERR("Init Main Video Stream_Storage Fail. %d", ret);
        return OPRT_COM_ERROR;
    }
    IPC_APP_set_sd_record_onoff(true);  //开启SD卡存储
    IPC_APP_set_sd_record_mode(0); //选择模式为事件存储模式（1代表连续存储）
    return OPRT_OK;
}

