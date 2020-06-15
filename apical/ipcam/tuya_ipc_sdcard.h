/*********************************************************************************
  *Copyright(C),2015-2020,
  *TUYA
  *www.tuya.comm
  *FileName:    tuya_ipc_sdcard.h
**********************************************************************************/


#ifndef _TUYA_IPC_SDCARD_H
#define _TUYA_IPC_SDCARD_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

VOID tuya_ipc_sd_format(VOID);
VOID tuya_ipc_sd_remount(VOID);
STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID);
E_SD_STATUS tuya_ipc_sd_get_status(VOID);
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free);
CHAR_T *tuya_ipc_get_sd_mount_path(VOID);
OPERATE_RET TUYA_APP_Init_Stream_Storage(IN CONST CHAR_T *p_sd_base_path);

#ifdef __cplusplus
}
#endif

#endif  /* _TUYA_IPC_MEDIA_UTILS_H */
