/*********************************************************************************
  *Copyright(C),2015-2020,
  *TUYA
  *www.tuya.comm
  *FileName:    tuya_ipc_main.h
**********************************************************************************/

#ifndef _TUYA_IPC_MAIN_H
#define _TUYA_IPC_MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int  tuya_ipc_main(char *uuid, char *key, char *token,int *exit_fun);
void tuya_audio(unsigned char *buf, int len);
void tuya_video(unsigned char *buf, int len, int sub);
void tuya_motion_event(int motion, uint32_t tick);
int  get_mqtt_status(void);

#ifdef __cplusplus
}
#endif

#endif  /* _TUYA_IPC_MEDIA_UTILS_H */
