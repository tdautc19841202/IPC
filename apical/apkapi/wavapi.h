#ifndef __WAVAPI_H__
#define __WAVAPI_H__

#include <stdint.h>

#define AI_DEV_ID   0
#define AI_CHN_ID0  0
#define AI_CHN_ID1  1
#define AO_DEV_ID   0
#define AO_CHN_ID   0

#define TOTAL_BUF_DEPTH     (8)

#define AI_SAMPRATE 8000  // 8000 or 16000
#define AO_SAMPRATE 8000  // 8000 or 16000
#define MI_AUDIO_SAMPLE_PER_FRAME 1024  //每个音频缓存帧的采样点个数768
#define DEF_AI_BUFSIZE (534 * (AI_SAMPRATE / 8000) * sizeof(int16_t) * 1)
#define DEF_AO_BUFSIZE (534 * (AI_SAMPRATE / 8000) * sizeof(int16_t) * 2)

#define ExecFunc(_func_, _ret_) \
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != _ret_) \
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)

#define ExecFuncNoExit(func, _ret_, __ret) \
do{	\
	MI_S32 s32TmpRet;	\
    printf("%d Start test: %s\n", __LINE__, #func);\
    s32TmpRet = func;	\
    __ret = s32TmpRet;	\
    if (s32TmpRet != _ret_)\
    {\
        printf("AUDIO_TEST [%d] %s exec function failed, result:0x%x\n",__LINE__, #func, s32TmpRet);\
    }\
    else\
    {\
        printf("AUDIO_TEST [%d] %s  exec function pass\n", __LINE__, #func);\
        printf("%d End test: %s\n", __LINE__, #func);	\
    }\
}while(0);

typedef enum
{
    E_AI_SOUND_MODE_MONO = 0,
    E_AI_SOUND_MODE_STEREO,
    E_AI_SOUND_MODE_QUEUE,
} AiSoundMode_e;

typedef enum
{
    E_AENC_TYPE_G711A = 0,
    E_AENC_TYPE_G711U,
    E_AENC_TYPE_G726_16,
    E_AENC_TYPE_G726_24,
    E_AENC_TYPE_G726_32,
    E_AENC_TYPE_G726_40,
    PCM,
} AencType_e;

typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;

int wavein_init(int ftest);
void wavein_exit(void);

int waveout_init  (void);
void waveout_exit  (void);
void waveout_file  (char *file);
void waveout_buffer(char *buf, int len, int alawulaw); // len must <= 512

uint8_t pcm2alaw(int16_t pcm );
int16_t alaw2pcm(uint8_t alaw);

#endif
