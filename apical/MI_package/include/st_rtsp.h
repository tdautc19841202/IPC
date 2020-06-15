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
#include "linux_list.h"
#include "mi_rgn.h"
#include "mi_divp.h"

#include "mi_od.h"
#include "mi_md.h"

#include "mi_vdf.h"
#include "mi_ao.h"
#include "mi_aio_datatype.h"
#include "mi_isp.h"
//
//#include "liveMedia.hh"
//#include "Live555RTSPServer.hh"

//#include "mi_iqserver.h"

#define RTSP_LISTEN_PORT        554
#define MAIN_STREAM                "main_stream"
#define SUB_STREAM0                "sub_stream0"
#define SUB_STREAM1                "sub_stream1"

#define RGN_OSD_HANDLE                    0
#define RGN_OSD_HANDLE1                   1
#define RGN_OSD_HANDLE2                   2

#define PATH_PREFIX                "/mnt"

#define RGN_OSD_TIME_START        8
#define RGN_OSD_MAX_NUM         4
#define RGN_OSD_TIME_WIDTH        180
#define RGN_OSD_TIME_HEIGHT        32

#define DOT_FONT_FILE            "/customer/mi_demo/gb2312.hzk"

#define MAX_CAPTURE_NUM            4
#define CAPTURE_PATH            "/mnt/capture"

#define BUF_POOL_SIZE 2048 * 1024 // 2mb

#define USE_AUDIO                 1
#define MI_AUDIO_SAMPLE_PER_FRAME 768
#define AO_INPUT_FILE            "8K_16bit_STERO_30s.wav"
#define AO_OUTPUT_FILE            "./tmp.pcm"

#define DIVP_CHN_FOR_OSD        0
#define DIVP_CHN_FOR_DLA        1
#define DIVP_CHN_FOR_VDF        2
#define DIVP_CHN_FOR_ROTATION    3
#define DIVP_CHN_FOR_SCALE        3
#define VENC_CHN_FOR_CAPTURE    12

#define SCALE_TEST    0
#define USE_STREAM_FILE 0
#define DIVP_CHN_FOR_RESOLUTION 0
#define DIVP_SCALE_IPNUT_FILE    "vpe0_port0_1920x1088_0000.yuv"

#define MAX_CHN_NEED_OSD        4

#define RAW_W                     384
#define RAW_H                     288
#define SHOW_PANEL 0
#define PANEL_DIVP_ROTATE        0

#define RGB_TO_CRYCB(r, g, b)                                                            \
        (((unsigned int)(( 0.439f * (r) - 0.368f * (g) - 0.071f * (b)) + 128.0f)) << 16) |    \
        (((unsigned int)(( 0.257f * (r) + 0.564f * (g) + 0.098f * (b)) + 16.0f)) << 8) |        \
        (((unsigned int)((-0.148f * (r) - 0.291f * (g) + 0.439f * (b)) + 128.0f)))

#define IQ_FILE_PATH    "/config/iqfile/day_wdr.bin"

typedef void (*ST_UserCmdProcess)(void *args);

#if (SHOW_PANEL == 1)
#include "mi_disp.h"
#include "mi_panel.h"
//#include "st_spi.h"
//#include "spi_cmd_480x854.h"
//#include "LX50FWB4001_RM68172_480x854.h"
//#include "AT070TN94_800x480.h"

#define ST_PANEL_WIDTH                480
#define ST_PANEL_HEIGHT                854
#endif

#define MAX_RGN_COVER_W               8192
#define MAX_RGN_COVER_H               8192

#define RGN_OSD_HANDLE                    0
#define RGN_FOR_VDF_BEGIN                12

#define MAX_FULL_RGN_NULL                3

#define ENABLE_BUF_POOL                1

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
typedef void (*ST_UserCmdProcess)(void *args);
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
    MI_U32     u32Height;
    MI_U32    u32CropX;
    MI_U32    u32CropY;
    MI_U32    u32CropWidth;
    MI_U32     u32CropHeight;

    MI_U32 enFunc;
    const char    *pszStreamName;
    MI_SYS_BindType_e eBindType;
    MI_U32 u32BindPara;
    MI_U32         u32Cover1Handle;
    MI_U32         u32Cover2Handle;
};

typedef struct
{
    MI_VENC_CHN vencChn;
    MI_VENC_ModType_e enType;
    char szStreamName[64];

    MI_BOOL bWriteFile;
    int fd;
    char szDebugFile[128];
} ST_StreamInfo_T;

typedef struct
{
    MI_VENC_CHN vencChn;
    pthread_t pt;
    MI_BOOL bRun;
} ST_CatpureJPGInfo_T;

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
#if USE_AUDIO
    MI_S32 s32UseAudio;        // 0: not use, else use
#endif
    MI_S32 s32LoadIQ;        // 0: not load, else load IQ file
#if (SHOW_PANEL == 1)
    MI_S32 s32UsePanel;        // 0: not use, else use
    MI_SYS_PixelFormat_e enPixelFormat;
    MI_S32 s32Rotate;        // 0: not rotate, else rotate
#endif
    MI_S32 s32HDRtype;
    ST_Sensor_Type_T enSensorType;
    MI_SYS_Rotate_e enRotation;
    MI_VPE_3DNR_Level_e en3dNrLevel;
} ST_Config_S;

typedef struct
{
    char szDesc[256];
    ST_UserCmdProcess process;
    void *args;
    int cmdID;

    struct list_head list;
} ST_Input_Cmd_S;

typedef struct
{
    int count;

    struct list_head head;
} ST_InputCmd_Mng_S;

typedef struct
{
    MI_RGN_HANDLE hHandle;
    MI_RGN_ModId_e eModId;
    MI_U32 u32Chn;
    MI_U32 u32Port;
} ST_VDF_OSD_Info_T;

typedef struct _WavHeader_s
{
    MI_U8    riff[4];                // RIFF string
    MI_U32    ChunkSize;                // overall size of file in bytes
    MI_U8    wave[4];                // WAVE string
    MI_U8    fmt_chunk_marker[4];    // fmt string with trailing null char
    MI_U32    length_of_fmt;            // length of the format data
    MI_U16    format_type;            // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
    MI_U16    channels;                // no.of channels
    MI_U32    sample_rate;            // sampling rate (blocks per second)
    MI_U32    byterate;                // SampleRate * NumChannels * BitsPerSample/8
    MI_U16    block_align;            // NumChannels * BitsPerSample/8
    MI_U16    bits_per_sample;        // bits per sample, 8- 8bits, 16- 16 bits etc
    MI_U8    data_chunk_header [4];    // DATA string or FLLR string
    MI_U32    data_size;                // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} _WavHeader_t;

typedef struct _AoOutChn_s
{

    pthread_t pt;
    MI_BOOL bRunFlag;

    MI_AUDIO_DEV AoDevId;
    MI_AO_CHN AoChn;
    int fdWr;
    MI_BOOL bEndOfStream;
    MI_U32 u32InputFileSize;
    MI_U32 u32WriteTotalSize;
    MI_U32 u32InSampleRate;
    MI_U32 u32OutSampleRate;
} _AoOutChn_t;

typedef struct ST_MDOD_Area_s
{
    MI_U32 u32Chn;
    ST_Rect_T stArea;
} ST_MDOD_Area_T;

typedef struct
{
    MI_U16 u16VdfInWidth;
    MI_U16 u16VdfInHeight;
    MI_U16 u16stride;

    MI_U16 u16OdNum;            // od chn num
    MI_U16 u16MdNum;            // md chn num
    MI_U16 u16VgNum;            // vg chn num
    ST_MDOD_Area_T stOdArea[4];         // od detect area
    ST_MDOD_Area_T stMdArea[4];         // md detect area
    ST_MDOD_Area_T stVgArea[4];         // vg detect area
} ST_VdfChnArgs_t;

struct ST_VdfSetting_Attr_T
{
    ST_Sys_Input_E enInput;
    MI_U32  u32InputChn;
    MI_U32  u32InputPort;
    ST_VdfChnArgs_t stVdfArgs;
};

static ST_InputCmd_Mng_S g_inputCmdMng =
{
    .count = 0,
    .head = LIST_HEAD_INIT(g_inputCmdMng.head)
};

#if 1
typedef struct
{
    pthread_t pt;
    MI_BOOL bRunFlag;

    int fdRd;
    MI_AUDIO_DEV AoDevId;
    MI_AO_CHN AoChn;
    MI_U32 u32ChnCnt;
    MI_AUDIO_BitWidth_e eBitwidth; /*audio frame bitwidth*/
    MI_AUDIO_SoundMode_e eSoundmode; /*audio frame momo or stereo mode*/
    MI_AUDIO_SampleRate_e eInSampleRate;
    MI_AUDIO_SampleRate_e eSamplerate;
} ST_AoInInfo_T;
#endif

typedef struct
{
    pthread_t pt;
    MI_VDF_CHANNEL vdfChn;
    MI_BOOL bRunFlag;
    MI_VDF_WorkMode_e enWorkMode;
    MI_U16 u16Width;
    MI_U16 u16Height;
} VDF_Thread_Args_t;

static struct ST_VdfSetting_Attr_T g_stVdfSettingAttr[] =
{
    {
        .enInput = ST_Sys_Input_DIVP,
        .u32InputChn = DIVP_CHN_FOR_VDF,
        .u32InputPort = 0,
        .stVdfArgs =
        {
            .u16VdfInWidth = 384,
            .u16VdfInHeight = 288,
            .u16stride = 384,
            .u16OdNum = 0,
            .u16MdNum = 1,
        },
    },
};

static struct ST_Stream_Attr_T g_stStreamAttr[] =
{
    {
        .bEnable = TRUE,
        .enInput = ST_Sys_Input_VPE,
        .u32InputChn = 0,
        .u32InputPort = 0,
        .vencChn = 0,
        .eType = E_MI_VENC_MODTYPE_JPEGE,
        .f32Mbps = 2.0,
        .u32Width = 1920,
        .u32Height = 1080,
        .u32CropX = 0,
        .u32CropY = 0,
        .u32CropWidth = 0,
        .u32CropHeight = 0,
        .enFunc = ST_Sys_Func_RTSP,
        .pszStreamName = SUB_STREAM0,
        .eBindType = E_MI_SYS_BIND_TYPE_REALTIME,
        .u32BindPara = 0,
        .u32Cover1Handle = 1,
        .u32Cover2Handle = 2,
    },
    {
        .bEnable = TRUE,
        .enInput = ST_Sys_Input_VPE,
        .u32InputChn = 0,
        .u32InputPort = 1,
        .vencChn = 1,
        .eType = E_MI_VENC_MODTYPE_H264E,
        .f32Mbps = 2.0,
        .u32Width = 1920,
        .u32Height = 1080,
        .u32CropX = 0,
        .u32CropY = 0,
        .u32CropWidth = 0,
        .u32CropHeight = 0,
        .enFunc = ST_Sys_Func_RTSP,
        .pszStreamName = MAIN_STREAM,
        .eBindType = E_MI_SYS_BIND_TYPE_HW_RING,
        .u32BindPara = 1080,
        .u32Cover1Handle = 3,
        .u32Cover2Handle = 4,
    },
    {
        .bEnable = TRUE,
        .enInput = ST_Sys_Input_VPE,
        .u32InputChn = 0,
        .u32InputPort = 2,
        .vencChn = 2,
        .eType = E_MI_VENC_MODTYPE_H264E,
        .f32Mbps = 2.0,
        .u32Width = 720,
        .u32Height = 576,
        .u32CropX = 0,
        .u32CropY = 0,
        .u32CropWidth = 0,
        .u32CropHeight = 0,
        .enFunc = ST_Sys_Func_RTSP,
        .pszStreamName = SUB_STREAM1,
        .eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE,
        .u32BindPara = 0,
        .u32Cover1Handle = 5,
        .u32Cover2Handle = 6,
    },
    {
        .bEnable = FALSE,
        .enInput = ST_Sys_Input_VPE,
        .u32InputChn = 0,
        .u32InputPort = 3,
        .vencChn = 3,
        .eType = E_MI_VENC_MODTYPE_H264E,
        .f32Mbps = 2.0,
        .u32Width = 384,
        .u32Height = 288,
        .u32CropX = 0,
        .u32CropY = 0,
        .u32CropWidth = 0,
        .u32CropHeight = 0,
        .enFunc = ST_Sys_Func_BUTT,
        .pszStreamName = SUB_STREAM1,
        .eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE,
        .u32BindPara = 0,
        .u32Cover1Handle = 7,
        .u32Cover2Handle = 8,
    },
};
void canvas_lock(void);
void canvas_unlock(void);
void ST_DefaultArgs(ST_Config_S *pstConfig);
void ST_ResetArgs(ST_Config_S *pstConfig);
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