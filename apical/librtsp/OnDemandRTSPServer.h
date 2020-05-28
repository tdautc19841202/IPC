#ifndef __ONDEMAND_RTSP_SERVER_H__
#define __ONDEMAND_RTSP_SERVER_H__

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "rtspserver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int             audio_enctype; // 0 - alaw, 1 - aac
    int             video_enctype; // 0 - h264, 1 - h265
    uint8_t         aac_config[2]; // aac_config
    pthread_t       pthread;
    char            bexit;
    int             kframe_dropped;
    int             ahead;
    int             atail;
    int             asize;
    pthread_mutex_t amutex;
    pthread_cond_t  acond;
    int             vhead;
    int             vtail;
    int             vsize;
    int             running_streams;
    pthread_mutex_t vmutex;
    pthread_cond_t  vcond;
    uint8_t         abuf[2056]; // (1024 + 4) * 2 = 2056
    uint8_t         vbuf[256*1024];
} CONTEXT;

int  rtsp_servermain(CONTEXT *context, char *pexit);
int  rtsp_get_aframe(CONTEXT *context, uint8_t *buf, int size, int *ndata);
int  rtsp_get_vframe(CONTEXT *context, uint8_t *buf, int size, int *ndata);
void ipcam_request_idr(int  main);

#ifdef __cplusplus
}
#endif

#endif
