#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "OnDemandRTSPServer.h"
#include "rtspserver.h"

extern int pthread_setname_np(pthread_t __target_thread, const char *__name);
extern int h264_parse_key_sps_pps(uint8_t *data, int len, int *key, uint8_t **sps_buf, int *sps_len, uint8_t **pps_buf, int *pps_len);

#define ENABLE_H264_AUTO_FRAME_DROP  1

static void* rtsp_server_thread_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    pthread_setname_np(pthread_self(), "rtsp");
    rtsp_servermain(context, &context->bexit);
    return NULL;
}

void* rtspserver_init(int aenc_type, int venc_type, uint8_t *aac_config)
{
    CONTEXT *context = NULL;
    pthread_attr_t attr;

    context = malloc(sizeof(CONTEXT));
    if (!context) return NULL;

    memset(context, 0, sizeof(CONTEXT) - sizeof(context->abuf) - sizeof(context->vbuf));
    context->audio_enctype = aenc_type;
    context->video_enctype = venc_type;

    if (aac_config) {
        context->aac_config[0] = aac_config[0];
        context->aac_config[1] = aac_config[1];
    }

    // init mutex & cond
    pthread_mutex_init(&context->amutex, NULL);
    pthread_mutex_init(&context->vmutex, NULL);
    pthread_cond_init (&context->acond , NULL);
    pthread_cond_init (&context->vcond , NULL);

    // create server thread
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 128 * 1024);
    pthread_create(&context->pthread, &attr, rtsp_server_thread_proc, context);
    return context;
}

void rtspserver_exit(void *ctx)
{
    CONTEXT *context = (CONTEXT*)ctx;
    if (!ctx) return;

    pthread_mutex_lock(&context->amutex);
    pthread_mutex_lock(&context->vmutex);
    context->bexit = 1;
    pthread_cond_signal(&context->acond);
    pthread_cond_signal(&context->vcond);
    pthread_mutex_unlock(&context->vmutex);
    pthread_mutex_unlock(&context->amutex);
    if (context->pthread) pthread_join(context->pthread, NULL);

    pthread_mutex_destroy(&context->amutex);
    pthread_mutex_destroy(&context->vmutex);
    pthread_cond_destroy (&context->acond );
    pthread_cond_destroy (&context->vcond );
    free(ctx);
}

static int cyclebuf_write(uint8_t *cbuf, int maxsize, int tail, uint8_t *src, int len)
{
    uint8_t *buf1 = cbuf    + tail;
    int      len1 = maxsize - tail < len ? maxsize - tail : len;
    uint8_t *buf2 = cbuf;
    int      len2 = len - len1;
    memcpy(buf1, src + 0   , len1);
    memcpy(buf2, src + len1, len2);
    return len2 ? len2 : tail + len1;
}

static int cyclebuf_read(uint8_t *cbuf, int maxsize, int head, uint8_t *dst, int len)
{
    uint8_t *buf1 = cbuf    + head;
    int      len1 = maxsize - head < len ? maxsize - head : len;
    uint8_t *buf2 = cbuf;
    int      len2 = len - len1;
    if (dst) memcpy(dst + 0   , buf1, len1);
    if (dst) memcpy(dst + len1, buf2, len2);
    return len2 ? len2 : head + len1;
}

void rtspserver_audio(void *ctx, uint8_t *data, int len)
{
    CONTEXT *context = (CONTEXT*)ctx;
    int      flag;
    if (!ctx) return;

    pthread_mutex_lock(&context->amutex);
    flag = len + sizeof(len) <= sizeof(context->abuf) - context->asize;
    pthread_mutex_unlock(&context->amutex);
    if (flag) {
        context->atail = cyclebuf_write(context->abuf, sizeof(context->abuf), context->atail, (uint8_t*)&len, sizeof(len));
        context->atail = cyclebuf_write(context->abuf, sizeof(context->abuf), context->atail, data, len);
        pthread_mutex_lock(&context->amutex);
        context->asize += len + sizeof(len);
        pthread_cond_signal(&context->acond);
        pthread_mutex_unlock(&context->amutex);
    }
}

void rtspserver_video(void *ctx, uint8_t *data, int len)
{
#if ENABLE_H264_AUTO_FRAME_DROP
    int kframe = 0;
#endif

    CONTEXT *context = (CONTEXT*)ctx;
    int      flag;
    if (!ctx) return;

#if ENABLE_H264_AUTO_FRAME_DROP
    if (context->video_enctype == 0) {
        h264_parse_key_sps_pps(data, len, &kframe, NULL, NULL, NULL, NULL);
        if (!kframe && context->kframe_dropped) {
//          printf("drop h264 non-key frame !\n");
            len = 0;
        }
    }
#endif

    pthread_mutex_lock(&context->vmutex);
    flag = len + sizeof(len) <= sizeof(context->vbuf) - context->vsize;
    pthread_mutex_unlock(&context->vmutex);
    if (flag) {
        context->vtail = cyclebuf_write(context->vbuf, sizeof(context->vbuf), context->vtail, (uint8_t*)&len, sizeof(len));
        context->vtail = cyclebuf_write(context->vbuf, sizeof(context->vbuf), context->vtail, data, len);
        pthread_mutex_lock(&context->vmutex);
        context->vsize += len + sizeof(len);
        pthread_cond_signal(&context->vcond);
        pthread_mutex_unlock(&context->vmutex);
#if ENABLE_H264_AUTO_FRAME_DROP
        if (context->video_enctype == 0 && kframe) {
            context->kframe_dropped = 0;
        }
#endif
    } else {
#if ENABLE_H264_AUTO_FRAME_DROP
        if (context->video_enctype == 0 && kframe) {
            context->kframe_dropped = 1;
//          printf("h264 key frame dropped !\n");
        }
#endif
    }
}

void rtspserver_reset(void *ctx)
{
    CONTEXT *context = (CONTEXT*)ctx;
    if (!ctx) return;
    pthread_mutex_lock(&context->amutex);
    pthread_mutex_lock(&context->vmutex);
    context->asize = context->vsize = 0;
    context->ahead = context->vhead = 0;
    context->atail = context->vtail = 0;
    context->kframe_dropped = 0;
    pthread_mutex_unlock(&context->vmutex);
    pthread_mutex_unlock(&context->amutex);
}

int rtsp_get_aframe(CONTEXT *context, uint8_t *buf, int size, int *ndata)
{
    int len, ncopy, flag;

    pthread_mutex_lock(&context->amutex);
//  printf("asize = %d\n", context->asize);
    flag = context->asize > 0 && !context->bexit;
    pthread_mutex_unlock(&context->amutex);
    if (!flag) { *ndata = 0; return 0; }

    context->ahead = cyclebuf_read(context->abuf, sizeof(context->abuf), context->ahead, (uint8_t*)&len, sizeof(len));
    *ndata = len;
    ncopy  = len < size ? len : size;

    context->ahead = cyclebuf_read(context->abuf, sizeof(context->abuf), context->ahead, buf, ncopy);
    context->ahead = cyclebuf_read(context->abuf, sizeof(context->abuf), context->ahead, NULL, len - ncopy);

    pthread_mutex_lock(&context->amutex);
    context->asize -= len + sizeof(len);
    pthread_mutex_unlock(&context->amutex);
    return ncopy;
}

int rtsp_get_vframe(CONTEXT *context, uint8_t *buf, int size, int *ndata)
{
    int len, ncopy;

    pthread_mutex_lock(&context->vmutex);
    while (context->vsize == 0 && !context->bexit) pthread_cond_wait(&context->vcond, &context->vmutex);
//  printf("vsize = %d\n", context->vsize);
    pthread_mutex_unlock(&context->vmutex);
    if (context->bexit) { *ndata = 0; return 0; }

    context->vhead = cyclebuf_read(context->vbuf, sizeof(context->vbuf), context->vhead, (uint8_t*)&len, sizeof(len));
    *ndata = len;
    ncopy  = len < size ? len : size;

    context->vhead = cyclebuf_read(context->vbuf, sizeof(context->vbuf), context->vhead, buf, ncopy);
    context->vhead = cyclebuf_read(context->vbuf, sizeof(context->vbuf), context->vhead, NULL, len - ncopy);

    pthread_mutex_lock(&context->vmutex);
    context->vsize -= len + sizeof(len);
    pthread_mutex_unlock(&context->vmutex);
    return ncopy;
}

int rtspserver_running_streams(void *ctx)
{
    CONTEXT *context = (CONTEXT*)ctx;
    return context ? context->running_streams : 0;
}
