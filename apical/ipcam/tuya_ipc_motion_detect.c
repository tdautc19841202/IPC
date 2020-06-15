/*********************************************************************************
  *Copyright(C),2015-2020,
  *TUYA
  *www.tuya.comm
  *FileName:    tuya_ipc_motion_detect_demo
**********************************************************************************/

#include <stdio.h>
#include <string.h>
#include "tuya_ipc_api.h"

extern int ipcam_take_snapshot(void **buf, int *size);

// AI detect should enable SUPPORT_AI_DETECT
#define SUPPORT_AI_DETECT 1
#if SUPPORT_AI_DETECT
#include "tuya_ipc_ai_detect_storage.h"
#endif

#if SUPPORT_AI_DETECT
// According to different chip platforms, users need to implement the interface of capture.
VOID tuya_ipc_get_snapshot_cb(char *pjbuf, int *size)
{
    void *buf;
    printf("tuya_ipc_get_snapshot_cb %p, %d\n", pjbuf, *size);
    ipcam_take_snapshot(&buf, size);
    memcpy(pjbuf, buf, *size);
}
#endif

#if SUPPORT_AI_DETECT
extern IPC_MEDIA_INFO_S s_media_info;
OPERATE_RET TUYA_APP_Enable_AI_Detect()
{
    tuya_ipc_ai_detect_storage_init(&s_media_info);

    return OPRT_OK;
}
#endif

