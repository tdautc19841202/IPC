#ifndef __RTSPSERVER_H__
#define __RTSPSERVER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* rtspserver_init (int aenc_type, int venc_type, uint8_t *aac_config);
void  rtspserver_exit (void *ctx);
void  rtspserver_audio(void *ctx, uint8_t *data, int len);
void  rtspserver_video(void *ctx, uint8_t *data, int len);
void  rtspserver_reset(void *ctx);

int   rtspserver_running_streams(void *ctx);

#ifdef __cplusplus
}
#endif

#endif







