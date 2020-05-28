#ifndef ST_RTSP
#define ST_RTSP
#include "mi_vpe_datatype.h"
#include "mi_venc_datatype.h"
#include "mi_common_datatype.h"
#include "mi_sys_datatype.h"
#include "st_common.h"
#include "st_rgn.h"
#include "st_vpe.h"
#include "zscanner.h"
#include <pthread.h>
#define MAX_CHN_NEED_OSD        4
#define MAIN_STREAM                "main_stream"
#define SUB_STREAM0                "sub_stream0"
#define SUB_STREAM1                "sub_stream1"
#define RGN_OSD_HANDLE                    0
#define RGN_OSD_HANDLE1                   1
#define RGN_OSD_HANDLE2                   2

#ifndef STCHECKRESULT
#define STCHECKRESULT(_func_)\
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != MI_SUCCESS)\
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__); \
        } \
    } while(0)
#endif

#ifdef ALIGN_UP
#undef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#else
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#endif
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif
#define MI_FAIL     1

#ifdef __cplusplus
extern "C" {
#endif

struct ST_Stream_Attr_T
{
    MI_BOOL        bEnable;
    ST_Sys_Input_E enInput;
    MI_U32     u32InputChn;
    MI_U32     u32InputPort;
    MI_VENC_CHN vencChn;
    MI_VENC_ModType_e eType;
    float     f32Mbps;
    MI_U32    u32Width;
    MI_U32    u32Height;
    MI_U32    u32FrameRate;
    MI_U32    u32CropX;
    MI_U32    u32CropY;
    MI_U32    u32CropWidth;
    MI_U32     u32CropHeight;

    MI_U32 enFunc;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindPara;
    MI_U32         u32Cover1Handle;
    MI_U32         u32Cover2Handle;
};

typedef struct
{
    void *pDataAddr;
    void *pDataAddrStart;
    void *pDataAddrAlign;
    MI_U32 u32DataLen;
}ST_DataPackage;

typedef struct
{
    pthread_t pt;
    MI_BOOL bRun;

    ST_OSD_INFO_S stOsdInfo[MAX_CHN_NEED_OSD][MAX_OSD_NUM_PER_CHN];
} ST_RGN_Osd_T;

typedef struct
{
    MI_S32 s32UseOnvif;     //0:not use, else use
    MI_S32 s32UseVdf;         // 0: not use, else use
// #if USE_AUDIO
    MI_S32 s32UseAudio;        // 0: not use, else use
// #endif
    MI_S32 s32LoadIQ;        // 0: not load, else load IQ file
#if (SHOW_PANEL == 1)
    MI_S32 s32UsePanel;        // 0: not use, else use
    MI_SYS_PixelFormat_e enPixelFormat;
    MI_S32 s32Rotate;        // 0: not rotate, else rotate
#endif
    MI_S32 s32HDRtype;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
} ST_Config_S;

static struct ST_Stream_Attr_T g_stStreamAttr[] =
{
    {
        FALSE, //bEnable
        ST_Sys_Input_DIVP, //enInput
        0, //u32InputChn
        0, //u32InputPort
        0, //vencChn
        E_MI_VENC_MODTYPE_JPEGE, //eType
        0.5, //f32Mbps
        1920, //u32Width
        1080, //u32Height
        0, //u32CropX
        0, //u32CropY
        15, //u32FrameRate
        0, //u32CropWidth
        0, //u32CropHeight
        ST_Sys_Func_RTSP, //enFunc
        E_MI_SYS_BIND_TYPE_REALTIME, //eBindType
        0, //u32BindPara
        1, //u32Cover1Handle
        2, //u32Cover2Handle
    },
    {
        TRUE, //bEnable
        ST_Sys_Input_VPE, //enInput
        0, //u32InputChn
        1, //u32InputPort
        1, //vencChn
        E_MI_VENC_MODTYPE_H264E, //eType
        1, //f32Mbps
        1920, //u32Width
        1080, //u32Height
        15, //u32FrameRate
        0, //u32CropX
        0, //u32CropY
        0, //u32CropWidth
        0, //u32CropHeight
        ST_Sys_Func_RTSP, //enFunc
        E_MI_SYS_BIND_TYPE_HW_RING, //eBindType
        1080, //u32BindPara
        3, //u32Cover1Handle
        4, //u32Cover2Handle
    },
    {
        TRUE, //bEnable
        ST_Sys_Input_DIVP, //enInput
        0, //u32InputChn
        2, //u32InputPort
        2, //vencChn
        E_MI_VENC_MODTYPE_H264E, //eType
        1.0, //f32Mbps
        704,//720, //u32Width
        396,//576, //u32Height
        15, //u32FrameRate
        0, //u32CropX
        0, //u32CropY
        0, //u32CropWidth
        0, //u32CropHeight
        ST_Sys_Func_RTSP, //enFunc
        E_MI_SYS_BIND_TYPE_FRAME_BASE, //eBindType
        0, //u32BindPara
        5, //u32Cover1Handle
        6, //u32Cover2Handle
    },
};
void canvas_lock(void);
void canvas_unlock(void);
void ST_ConfigSet(ST_Config_S *pstConfig);
MI_S32 ST_BaseModuleInit(ST_Config_S* pstConfig);
MI_S32 ST_BaseModuleUnInit(void);
MI_S32 ST_StartPipeLine(MI_U8 i, MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32CropW, MI_U32 u32CropH, MI_U32 u32CropX, MI_U32 u32CropY);
MI_S32 ST_StartPipeLineWithDip(MI_U8 i, MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32CropW, MI_U32 u32CropH, MI_U32 u32CropX, MI_U32 u32CropY);
MI_S32 ST_StopPipeLine(MI_U8 i);
MI_S32 ST_StopPipeLineWithDip(MI_U8 i);
MI_BOOL ST_DoCaptureJPGProc(MI_U16 u16Width, MI_U16 u16Height, MI_SYS_Rotate_e enRotation);
MI_BOOL ST_DoCaptureJPGProcExt(MI_U16 u16Width, MI_U16 u16Hight, MI_SYS_Rotate_e enRotation);
MI_S32 InitRGN();
void ST_CaptureJPGProc(MI_VENC_CHN VencChn);

#ifdef __cplusplus
}
#endif

#endif