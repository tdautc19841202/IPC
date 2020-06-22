#ifndef __AVKCPS_H__
#define __AVKCPS_H__

#ifdef WIN32
#define AVKCPSAPI __declspec(dllexport)
#else
#define AVKCPSAPI __attribute__((visibility ("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PFN_REQUEST_IDR)(int);
AVKCPSAPI void* avkcps_init (int port, char *aenctype, int channels, int samprate,
                             char *venctype, int width, int height, int frate,
                             int maxbufn, PFN_REQUEST_IDR pfnreqidr);
AVKCPSAPI void  avkcps_exit (void *ctxt);
AVKCPSAPI void  avkcps_audio(void *ctx, char *data, int len, int key);
AVKCPSAPI void  avkcps_video(void *ctx, char *data, int len, int key);

#ifdef __cplusplus
}
#endif

#endif
