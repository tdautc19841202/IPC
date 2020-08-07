#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <execinfo.h>
#include <arpa/inet.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/poll.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <fcntl.h>
#include "st_rgn.h"
#include "st_rtsp.h"
#include "wavapi.h"
#include "mi_rgn.h"
#include "mi_common.h"
#include "mi_venc.h"
#include "mi_divp.h"
#include "mi_od.h"
#include "mi_md.h"
#include "mi_vdf.h"
#include "mi_ai.h"
#include "mi_ao.h"
#include "mi_aio_datatype.h"
#include "mi_isp.h"
#include "mi_iqserver.h"
#include "apkapi.h"
#include "avkcps.h"
#include "settings.h"
#include "updateuid.h"
#include "mp3dec.h"
#include "tuya_ipc_main.h"
#include "st_uvc_datatype.h"
#include "motor.h"
#include "fft.h"
#include <errno.h>
#include "motiondet.h"
#include "st_rtsp.h"
extern int pthread_setname_np(pthread_t __target_thread, const char *__name);

#define WIFI_CONF_FILE        "/customer/data/cfg/wpa_supplicant.conf"
#define SETTINGS_FILE         "/customer/data/ipcam/ipcam.ini"
#define TUYA_DB_FILE1         "/customer/data/ipcam/tuya_user.db"
#define TUYA_DB_FILE2         "/customer/data/ipcam/tuya_user.db_bak"
#define BEEP_AUDIO_FILE       "/customer/res/1khz.mp3"
#define WELCOME_AUDIO_FILE    "/customer/res/welcome.mp3"
#define WIFI_HWOK_FILE        "/customer/res/wifihwok.mp3"
#define WIFI_HWNG_FILE        "/customer/res/wifihwng.mp3"
#define WIFI_TEST_FILE        "/customer/res/wifitest.mp3"
#define MICOK_AUDIO_FILE      "/customer/res/micok.mp3"
#define MICNG_AUDIO_FILE      "/customer/res/micng.mp3"
#define AGING_TEST_FILE       "/customer/res/agingtest.mp3"
#define SCANOK_AUDIO_FILE     "/customer/res/scanok.mp3"
#define DEVICE_CONNECTED_FILE "/customer/res/connected.mp3"
#define SDCARD_INSERT_FILE    "/customer/res/sdinsert.mp3"
#define SDCARD_EJECT_FILE     "/customer/res/sdeject.mp3"
#define SDCARD_UPDATE_UID_OK  "/customer/res/upduidok.mp3"
#define FIRMWARE_UPDATING     "/customer/res/fwupdating.mp3"
#define FIRMWARE_UPDATERR     "/customer/res/fwupdaterr.mp3"

#define OUTPUTFILENAME        "/tmp/test.wav" 

#define FACTORY_TEST_PORT     8313
#define VENC_CHN_FOR_CAPTURE    12
//#define TEST_BY_ZYH     1
char g_zbar_scan_str[64] = "";
typedef struct {
    int             main_vichn;
    int             sub0_vichn;
    int             sub1_vichn;
    int             main_vechn;
    int             sub0_vechn;

    #define FLAG_EXIT_VMAIN_THEADS   (1 << 0 )
    #define FLAG_EXIT_VSUB_THEADS    (1 << 1 )
    #define FLAG_EXIT_OTHER_THEADS   (1 << 2 )
    #define FLAG_WIFI_HWOK           (1 << 3 )
    #define FLAG_WIFI_CONNECTED      (1 << 4 )
    #define FLAG_ENABLE_ZSCANNER     (1 << 5 )
    #define FLAG_REQUEST_IDR_MAIN    (1 << 6 )
    #define FLAG_REQUEST_IDR_SUB     (1 << 7 )
    #define FLAG_VMAIN_INITED        (1 << 8 )
    #define FLAG_VSUB_INITED         (1 << 9 )
    #define FLAG_IPERF_STARTED       (1 << 11)
    #define FLAG_JPEG_SNAPSHOT       (1 << 12)
    #define FLAG_MP3_ISPLAYING       (1 << 13)
    #define FLAG_VOICE_ISPLAYING     (1 << 14)
    #define FLAG_SD_FIRST_INSERT     (1 << 15)
    #define FLAG_HAVE_PAIRED         (1 << 16)
    #define FLAG_CHECK_WLAN_MAP      (1 << 17)
    #define FLAG_GET_WLAN_INFO       (1 << 18)
    uint32_t        status;
    uint32_t        ledtype;

    pthread_attr_t  pthread_attr;
    pthread_t       pthread_main;
    pthread_t       pthread_sub ;
    pthread_t       pthread_scan;
    pthread_t       pthread_audc;
    pthread_t       pthread_nmon;
    pthread_t       pthread_dmon;
    pthread_t       pthread_play;
    pthread_t       pthread_test;
    pthread_t       pthread_mp3 ;
    pthread_t       pthread_rgn ;
    pthread_t       pthread_ptzm;
    pthread_t       pthread_jpeg;
    pthread_t       pthread_led ;
    pthread_t       pthread_avkcp ;

    struct tm       current_time;
    uint32_t        playstoptick;
    int             rec_buf_size;
    int             main_idr_counter;
    int             sub0_idr_counter;
    int             last_irmode;
    uint32_t        last_motion_record;
    uint32_t        last_motion_report;
    uint32_t        last_zbscan_tick;
    uint32_t        last_ntpfix_tick;
    uint32_t        spkmic_test_tick;
    int             error_counter;
    int             exit_tuya;
    int             ir_mode;                //0为自动 1为关闭 2为开启

    #define         HW_BTN_PRESSED  (1 << 0)
    #define         HW_BTN_UP_OK    (1 << 1)
    #define         HW_BTN_DOWN_OK  (1 << 2)
    #define         HW_LSEN_UP_OK   (1 << 3)
    #define         HW_LSEN_DOWN_OK (1 << 4)
    #define         HW_MIC_TEST_OK  (1 << 5)
    #define         HW_MIC_TESTED   (1 << 6)
    #define         HW_SD_TEST_OK   (1 << 7)
    #define         HW_LIGHTCAM_OK  (1 << 8)
    uint32_t        hwstate;

    pthread_mutex_t lock0;
    pthread_mutex_t lock1;

    void           *mp3dec;
    void           *aacenc;
    void           *zscanner;
    void           *motion;
    FILE           *fprec;
    void           *avkcps;
    void           *recorder;
    uint32_t        rectick;
    void           *motor;
    void           *lightcam;
    void           *fft;

    struct tagcloudmeetapplication *pcloudmeet_application;
    IPCAMSETTINGS   settings;
    ST_Config_S     pstConfig;
    ST_RGN_Osd_T    g_stRgnOsd;
    char            user_account[33];
    char            devuid      [33];
    char            devsid      [33];
    char            devsn       [11];
    char            wifiap      [33];
    char            passwd      [65];
    char            mp3file     [64];
    char            ispbinday   [32];
    char            ispbinight  [32];
    SDSTATUS       *sdstatus;

    uint8_t         snapshot_buf[64 * 1024];
    int32_t         snapshot_len;
    int32_t         ptz_move;
} CONTEXT;
static CONTEXT g_app_ctx = { 0, 3, 4, 0, 3 };

static void main_video_rawrec(CONTEXT *context, MI_VENC_Stream_t *stream)
{
    if (context->settings.rawrec_en) {
        if (context->fprec == NULL) {
            context->fprec = fopen(context->settings.main_venc_type ? SDCARD_MOUNT_PATH"/rawrec/raw.h265" : SDCARD_MOUNT_PATH"/rawrec/raw.h264", "wb");
        }
        if (context->fprec != NULL) {
            fwrite(stream->pstPack->pu8Addr, 1, stream->pstPack->u32Len, context->fprec);
            if (fflush(context->fprec) == EOF) {
                fclose(context->fprec);
                context->fprec = NULL;
            }
        }
    } else {
        if (context->fprec) {
            fclose(context->fprec);
            context->fprec = NULL;
        }
    }
}

static void* jpeg_snapshot_proc(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack;

    memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
    memset(&stPack, 0, sizeof(MI_VENC_Pack_t));
    stStream.pstPack = &stPack;
    stStream.u32PackCount = 1;
    ST_DoCaptureJPGProc(704, 396, E_MI_SYS_ROTATE_NONE);
    s32Ret = MI_VENC_StartRecvPic(VENC_CHN_FOR_CAPTURE);
    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_VENC_StartRecvPic fail, 0x%x\n", s32Ret);
    }
    while(!(context->status & FLAG_EXIT_VSUB_THEADS))
    {
        if(context->status & FLAG_WIFI_CONNECTED)
        {   
            s32Ret = MI_VENC_GetStream(VENC_CHN_FOR_CAPTURE, &stStream, 40);
            if (context->status & FLAG_JPEG_SNAPSHOT && MI_SUCCESS == s32Ret)
            {
                context->status &= ~FLAG_JPEG_SNAPSHOT;
                context->snapshot_len = stStream.pstPack->u32Len;
                memcpy(context->snapshot_buf, stStream.pstPack->pu8Addr, MIN(sizeof(context->snapshot_buf), stStream.pstPack->u32Len));
            }
            else
            {
                usleep(200 * 1000);
            }
            MI_VENC_ReleaseStream(VENC_CHN_FOR_CAPTURE, &stStream);   
        }
        else
        {
            usleep(200 * 1000);
        }
    }
    s32Ret = MI_VENC_StopRecvPic(VENC_CHN_FOR_CAPTURE);
    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("ST_Venc_StopChannel fail, 0x%x\n", s32Ret);
    }
}

static void* mp3_playback_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    pthread_setname_np(pthread_self(), "mp3");
    if (context->playstoptick != 1) {
        if (context->playstoptick == 0) set_gpio(GPIO_SPK_PWR, 1);
        context->playstoptick = 1;
        mp3dec_play_file(context->mp3dec, context->mp3file);
        context->playstoptick = get_tick_count();
    }
    pthread_detach(pthread_self());
    context->pthread_mp3 = (pthread_t)NULL;
    return NULL;
}

static void play_mp3_file(CONTEXT *context, char *file, int wait)
{
    if (context->settings.standby || strcmp(context->settings.ft_mode, "uvc") == 0) {
        return;
    }
    while (wait && context->pthread_mp3) {
        usleep(100*1000);
    }
    if (file && !context->pthread_mp3) {
        strncpy(context->mp3file, file, sizeof(context->mp3file));
        pthread_create(&context->pthread_mp3, NULL, mp3_playback_proc, context);
    }
}

static void update_datetime(CONTEXT *context)
{
    time_t t = time(NULL) + context->settings.watermark_timezone;
    localtime_r(&t, &context->current_time);
}

void *UpdateRgnOsdTimeProc(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    time_t now = 0;
    struct tm *tm = NULL;
    char szTime[64];
    int len = 0;
    struct timeval tv;
    MI_RGN_CanvasInfo_t* pstCanvasInfo = NULL;
    ST_Point_T stPoint;
    time_t defaultInterval = 980 * 1000;
    struct timeval timeBegin, timeEnd;
    MI_RGN_ChnPort_t stChnPort;
    MI_RGN_ChnPortParam_t stChnPortParam;
    bool CLOSE_OSD_FLAG = 0;
    pthread_setname_np(pthread_self(), "osd");

    while (!(context->status & FLAG_EXIT_OTHER_THEADS))
    {
        if(context->status & FLAG_WIFI_CONNECTED)
        {
            if( context->settings.watermark_visible == 1)
            {
                update_datetime(context);
                tm = &context->current_time;
                memset(szTime, 0, sizeof(szTime));
                len = 0;
                len += sprintf(szTime + len, "%d-", tm->tm_year + 1900);
                len += sprintf(szTime + len, "%02d-", tm->tm_mon + 1);
                len += sprintf(szTime + len, "%02d  ", tm->tm_mday);
                len += sprintf(szTime + len, "%02d:", tm->tm_hour);
                len += sprintf(szTime + len, "%02d:", tm->tm_min);
                len += sprintf(szTime + len, "%02d  ", tm->tm_sec);
#if 0                
                len += sprintf(szTime + len, "LV=%d     ", context->settings.soft_light_sensor_LV);
                len += sprintf(szTime + len, "SG=%d     ", context->settings.soft_light_sensor_SensorGain);
                len += sprintf(szTime + len, "AWB=%f       ", context->settings.soft_light_sensor_AWB);
                len += sprintf(szTime + len, "day2night LV<%d SG>%d   ", context->settings.dSensor_LV, context->settings.dSensor_Gain);
                len += sprintf(szTime + len, "night2day LV>%d SG<%d AWB>%f   ", context->settings.nSensor_LV, context->settings.nSensor_Gain, context->settings.nSensor_AWB);
                len += sprintf(szTime + len, "ir_en=%d", context->settings.ir_en);
#endif                
                stPoint.u32X = 0;
                stPoint.u32Y = 0;

                canvas_lock();
                CLOSE_OSD_FLAG = 0;
                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_VPE;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 0;
                stChnPort.s32OutputPortId = 1;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = TRUE;        //osd show
                //stPoint of time_osd   zyh_add
                stChnPortParam.stPoint.u32X = 10;
                stChnPortParam.stPoint.u32Y = 10;
                stChnPortParam.unPara.stCoverChnPort.u32Layer = 100;
                stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 0;
                stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 0;
                stChnPortParam.unPara.stCoverChnPort.u32Color = 0;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE, &stChnPort, &stChnPortParam);

                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_DIVP;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 5;
                stChnPort.s32OutputPortId = 0;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = TRUE;       //osd show 
                stChnPortParam.stPoint.u32X = 2;
                stChnPortParam.stPoint.u32Y = 2;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE1, &stChnPort, &stChnPortParam);

                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_DIVP;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 0;
                stChnPort.s32OutputPortId = 0;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = TRUE;       //osd show 
                stChnPortParam.stPoint.u32X = 0;
                stChnPortParam.stPoint.u32Y = 0;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE2, &stChnPort, &stChnPortParam);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE, NULL);
                (void)ST_OSD_DrawText(RGN_OSD_HANDLE, stPoint, szTime, I4_WHITE, DMF_Font_Size_48x48);
                (void)ST_OSD_Update(RGN_OSD_HANDLE);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE1, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE1, NULL);
                (void)ST_OSD_DrawText(RGN_OSD_HANDLE1, stPoint, szTime, I4_WHITE, DMF_Font_Size_32x32);
                (void)ST_OSD_Update(RGN_OSD_HANDLE1);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE2, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE2, NULL);
                (void)ST_OSD_DrawText(RGN_OSD_HANDLE2, stPoint, szTime, I4_WHITE, DMF_Font_Size_32x32);
                (void)ST_OSD_Update(RGN_OSD_HANDLE2);
                usleep(500 * 1000);
            
                canvas_unlock();
            }
            if( context->settings.watermark_visible == 0 && CLOSE_OSD_FLAG == 0)
            {
                canvas_lock();
                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_VPE;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 0;
                stChnPort.s32OutputPortId = 1;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = FALSE;        //osd not show 
                //stPoint of time_osd   zyh_add
                stChnPortParam.stPoint.u32X = 10;
                stChnPortParam.stPoint.u32Y = 10;
                stChnPortParam.unPara.stCoverChnPort.u32Layer = 100;
                stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 0;
                stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 0;
                stChnPortParam.unPara.stCoverChnPort.u32Color = 0;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE, &stChnPort, &stChnPortParam);

                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_DIVP;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 5;
                stChnPort.s32OutputPortId = 0;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = FALSE;        //osd not show 
                stChnPortParam.stPoint.u32X = 2;
                stChnPortParam.stPoint.u32Y = 2;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE1, &stChnPort, &stChnPortParam);

                memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
                stChnPort.eModId = E_MI_RGN_MODID_DIVP;
                stChnPort.s32DevId = 0;
                stChnPort.s32ChnId = 0;
                stChnPort.s32OutputPortId = 0;
                memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
                stChnPortParam.bShow = FALSE;        //osd not show 
                stChnPortParam.stPoint.u32X = 2;
                stChnPortParam.stPoint.u32Y = 2;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
                stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
                MI_RGN_SetDisplayAttr(RGN_OSD_HANDLE2, &stChnPort, &stChnPortParam);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE, NULL);
                //(void)ST_OSD_DrawText(RGN_OSD_HANDLE, stPoint, szTime, I4_RED, DMF_Font_Size_48x48);
                (void)ST_OSD_Update(RGN_OSD_HANDLE);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE1, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE1, NULL);
                //(void)ST_OSD_DrawText(RGN_OSD_HANDLE1, stPoint, szTime, I4_RED, DMF_Font_Size_32x32);
                (void)ST_OSD_Update(RGN_OSD_HANDLE1);

                (void)ST_OSD_GetCanvasInfo(RGN_OSD_HANDLE2, &pstCanvasInfo);
                (void)ST_OSD_Clear(RGN_OSD_HANDLE2, NULL);
                //(void)ST_OSD_DrawText(RGN_OSD_HANDLE1, stPoint, szTime, I4_RED, DMF_Font_Size_32x32);
                (void)ST_OSD_Update(RGN_OSD_HANDLE2);
           
                CLOSE_OSD_FLAG = 1;
                canvas_unlock();
            }
        }
        else
        {
            usleep(200*1000);continue;
        }
    }
    return NULL;
}

static void * main_stream(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    struct ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_U32 i = 1;  //选择主码流
    MI_SYS_ChnPort_t stVencChnPort;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_Stream_t vstream;
    MI_VENC_Pack_t stPack[16];
    MI_VENC_ChnStat_t stStat;
    MI_SYS_ChnPort_t *pstChnPort;
    MI_S32 s32Fd;
    fd_set read_fds;

    pthread_setname_np(pthread_self(), "main_str");
    if (pstStreamAttr[i].bEnable == FALSE)
    {
        printf("pstStreamAttr[%d] is not Enable！\n",i);
        return NULL;
    }
    if(pstStreamAttr[i].enInput == ST_Sys_Input_VPE)
    {
        ST_StartPipeLine(i, pstStreamAttr[i].u32Width, pstStreamAttr[i].u32Height, pstStreamAttr[i].u32CropWidth, pstStreamAttr[i].u32CropHeight, pstStreamAttr[i].u32CropX, pstStreamAttr[i].u32CropY);
    }
    else if (pstStreamAttr[i].enInput == ST_Sys_Input_DIVP)
    {
        ST_StartPipeLineWithDip(i, pstStreamAttr[i].u32Width, pstStreamAttr[i].u32Height, pstStreamAttr[i].u32CropWidth, pstStreamAttr[i].u32CropHeight, pstStreamAttr[i].u32CropX, pstStreamAttr[i].u32CropY);
    }
    if(i == 1)
    {
        // ST_OpenBufPool(pstStreamAttr[i].vencChn);
        MI_VENC_CHN stVencChn = pstStreamAttr[i].vencChn;
        memset(&stVencChnPort, 0, sizeof(MI_SYS_ChnPort_t));
        stVencChnPort.eModId = E_MI_MODULE_ID_VENC;
        stVencChnPort.u32ChnId = stVencChn;
        stVencChnPort.u32PortId = 0;
        if(MI_VENC_GetChnDevid(stVencChn, &stVencChnPort.u32DevId) != MI_SUCCESS){
            printf("MI_VENC_GetChnDevid failed!\n");
            return NULL;
        }

        pstChnPort = (MI_SYS_ChnPort_t *)&stVencChnPort;
        s32Fd = MI_VENC_GetFd((MI_VENC_CHN)pstChnPort->u32ChnId);
        if (s32Fd < 0)
            return NULL;

        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);
        context->status |= FLAG_VMAIN_INITED;
        int ftest = strcmp(context->settings.ft_mode, "");
        while(!(context->status & FLAG_EXIT_OTHER_THEADS))
        {
            if(context->status & FLAG_WIFI_CONNECTED)
            {
                memset(&vstream, 0, sizeof(vstream));
                memset(&stPack, 0, sizeof(stPack));
                vstream.pstPack = stPack;
                s32Ret = MI_VENC_Query((MI_VENC_CHN)pstChnPort->u32ChnId, &stStat);
                //printf("MI_VENC_Query finish,s32Ret = %d,stStat.u32CurPacks = %d\n\n",s32Ret,stStat.u32CurPacks);
                if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
                {
                    usleep(100*1000); 
                    continue;
                }
                vstream.u32PackCount = stStat.u32CurPacks;
                if (context->status & FLAG_REQUEST_IDR_MAIN)
                {
                    MI_VENC_RequestIdr((MI_VENC_CHN)pstChnPort->u32ChnId, 1);
                }
                s32Ret = MI_VENC_GetStream((MI_VENC_CHN)pstChnPort->u32ChnId, &vstream, -1);
                if(MI_SUCCESS == s32Ret)
                {
                    if (!ftest)
                    {
                        tuya_video(vstream.pstPack->pu8Addr, vstream.pstPack->u32Len, 0);
                    }
                    else
                    {
                        if (context->status & FLAG_REQUEST_IDR_MAIN)
                        {
                            avkcps_video(context->avkcps, vstream.pstPack->pu8Addr, vstream.pstPack->u32Len, 1);
                        }
                        else
                        {
                            avkcps_video(context->avkcps, vstream.pstPack->pu8Addr, vstream.pstPack->u32Len, 0);
                        }    
                    }
                }
                else{
                    printf("MI_VENC_GetStream failed\n");
                    usleep(100*1000); continue;
                }
                MI_VENC_ReleaseStream((MI_VENC_CHN)pstChnPort->u32ChnId, &vstream);
                context->g_stRgnOsd.bRun = TRUE;
                context->status &= ~FLAG_REQUEST_IDR_MAIN;
           }
            else
            {
                usleep(100*1000);continue;
            }  
        }
        context->g_stRgnOsd.bRun = FALSE;
        if(pstStreamAttr[i].enInput == ST_Sys_Input_VPE)
        {
            // main_video_rawrec(context, &vstream);
            // tuya_video(vstream.pstPack->pu8Addr, vstream.pstPack->u32Len, 0);
            ST_StopPipeLine(i);
        }
        else if (pstStreamAttr[i].enInput == ST_Sys_Input_DIVP)
        {
            ST_StopPipeLineWithDip(i);
        }
        MI_VENC_CloseFd(s32Fd);
    }

    return NULL;
}

static void handle_zbar_scan(CONTEXT *context, uint8_t *data)
{
    char *str;
    str = zscanner_scan(context->zscanner, (char*)data);
    if (str) {
        play_mp3_file(context, SCANOK_AUDIO_FILE, 0);
        strncpy(g_zbar_scan_str, str, sizeof(g_zbar_scan_str));
        context->status &= ~FLAG_ENABLE_ZSCANNER;
    }
}

static void * sub_stream(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    struct ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_U32 i = 2;  //选择子码流
    MI_SYS_ChnPort_t stVencChnPort;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_Stream_t vstream;
    MI_VENC_Pack_t stPack[16];
    MI_VENC_ChnStat_t stStat;
    MI_SYS_ChnPort_t *pstChnPort;
    MI_S32 s32Fd;
    fd_set read_fds;

    pthread_setname_np(pthread_self(), "sub_stream");
    if (pstStreamAttr[i].bEnable == FALSE)
    {
        printf("pstStreamAttr[%d] is not Enable！\n",i);
        return NULL;
    }
    if(!(context->status&FLAG_VSUB_INITED)){
        s32Ret = ST_StartPipeLineWithDip(i, ZBAR_VIDEO_WIDTH, ZBAR_VIDEO_HEIGHT, pstStreamAttr[i].u32CropWidth, pstStreamAttr[i].u32CropHeight, pstStreamAttr[i].u32CropX, pstStreamAttr[i].u32CropY);
        if(s32Ret == MI_SUCCESS)context->status |= FLAG_VSUB_INITED;
    }
    MI_VENC_CHN stVencChn = pstStreamAttr[i].vencChn;
    memset(&stVencChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stVencChnPort.eModId = E_MI_MODULE_ID_VENC;
    stVencChnPort.u32ChnId = stVencChn;
    stVencChnPort.u32PortId = 0;
    if(MI_VENC_GetChnDevid(stVencChn, &stVencChnPort.u32DevId) != MI_SUCCESS){
        printf("MI_VENC_GetChnDevid failed!\n");
        return NULL;
    }

    pstChnPort = (MI_SYS_ChnPort_t *)&stVencChnPort;
    s32Fd = MI_VENC_GetFd((MI_VENC_CHN)pstChnPort->u32ChnId);
    if (s32Fd < 0)
        return NULL;

    FD_ZERO(&read_fds);
    FD_SET(s32Fd, &read_fds);
    int ftest = strcmp(context->settings.ft_mode, "");
    while(!(context->status & FLAG_EXIT_OTHER_THEADS))
    {
        if(context->status & FLAG_WIFI_CONNECTED)
        {
            memset(&vstream, 0, sizeof(vstream));
            memset(&stPack, 0, sizeof(stPack));
            vstream.pstPack = stPack;
            s32Ret = MI_VENC_Query((MI_VENC_CHN)pstChnPort->u32ChnId, &stStat);
            // printf("MI_VENC_Query finish,s32Ret = %d,stStat.u32CurPacks = %d\n\n",s32Ret,stStat.u32CurPacks);
            if(s32Ret != MI_SUCCESS || stStat.u32CurPacks == 0)
            {
                usleep(100*1000);
                continue;
            }
            vstream.u32PackCount = stStat.u32CurPacks;
            s32Ret = MI_VENC_GetStream((MI_VENC_CHN)pstChnPort->u32ChnId, &vstream, -1);
            if(MI_SUCCESS == s32Ret)
            {
                if (!ftest) {
                    tuya_video(vstream.pstPack->pu8Addr, vstream.pstPack->u32Len, 1);
                }
            }
            else{
                printf("MI_VENC_GetStream failed\n");
                usleep(100*1000); continue;
            }
            MI_VENC_ReleaseStream((MI_VENC_CHN)pstChnPort->u32ChnId, &vstream);
            context->g_stRgnOsd.bRun = TRUE;
        }
        else
        {
            usleep(100*1000);continue;
        }  
    }
    context->g_stRgnOsd.bRun = FALSE;
    ST_StopPipeLineWithDip(i);
    MI_VENC_CloseFd(s32Fd);

    return NULL;
}

static void handle_motion_det(CONTEXT *context, uint8_t *data, int diff)
{
    struct tm *tmnow = &context->current_time;
    int        hmin  = (context->settings.md_timeperiod >> 8) & 0xff;
    int        hmax  = (context->settings.md_timeperiod >> 0) & 0xff;
    if (tmnow->tm_hour >= hmin && tmnow->tm_hour <= hmax) {
        int motion = motion_detect_run(context->motion, (char*)data, diff);
        tuya_motion_event(motion, get_tick_count());
    }
}

static void set_iperf3_start(CONTEXT *context, int start)
{
    if (start) {
        if (!(context->status & FLAG_IPERF_STARTED)) {
            context->status |= FLAG_IPERF_STARTED;
            system("iperf3 -s &");
        }
    } else {
        if (context->status & FLAG_IPERF_STARTED) {
            context->status &=~FLAG_IPERF_STARTED;
            system("killall iperf3");
        }
    }
}

static void * scan_thread(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    pthread_setname_np(pthread_self(), "scan&motion");
    context->zscanner = zscanner_init(ZBAR_VIDEO_WIDTH, ZBAR_VIDEO_HEIGHT, (ZBAR_VIDEO_WIDTH - 560) / 2, 0, 560, ZBAR_VIDEO_HEIGHT);
    context->motion   = motion_detect_init(2);
    MI_U32 u32VpePort = 2;
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BUF_HANDLE bufHandle;
    MI_SYS_BufInfo_t stBufInfo;
    MI_S32 s32Ret = MI_SUCCESS;

    while(!(context->status & FLAG_EXIT_VSUB_THEADS)){
        while(!(context->status&FLAG_VSUB_INITED)){
            usleep(500*1000);
            continue;
        }
        /*********控制在5帧**********/
        static uint32_t last_tick  = 0  ;
        static  int32_t sleep_tick = 100;
        uint32_t cur_tick = get_tick_count();
        uint32_t target   = 200;
        uint32_t diff     = cur_tick - last_tick;
        if      (diff > target + 100) sleep_tick -= 90;
        else if (diff > target + 50 ) sleep_tick -= 30;
        else if (diff > target + 5  ) sleep_tick -= 2 ;
        else if (diff < target - 100) sleep_tick += 90;
        else if (diff < target - 50 ) sleep_tick += 30;
        else if (diff < target - 5  ) sleep_tick += 2 ;
        sleep_tick = sleep_tick > 0 ? sleep_tick : 0;
        last_tick  = cur_tick;
        if (sleep_tick > 0) usleep(sleep_tick*1000);
        /*****************************/

        memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));

        stChnPort.eModId = E_MI_MODULE_ID_VPE;
        stChnPort.u32DevId = 0;
        stChnPort.u32ChnId = 0;
        stChnPort.u32PortId = u32VpePort;
        
        s32Ret = MI_SYS_SetChnOutputPortDepth(&stChnPort, 3, 4);
        if (MI_SUCCESS != s32Ret)
        {
            ST_ERR("MI_SYS_SetChnOutputPortDepth err:%x, chn:%d,port:%d\n", s32Ret,stChnPort.u32ChnId, stChnPort.u32PortId);
            usleep(100*1000);
            return NULL;
        }
        memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
        if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &bufHandle))
        {
            usleep(100*1000);
            continue;
        }
        if(context->status & FLAG_ENABLE_ZSCANNER) {
            handle_zbar_scan(context,stBufInfo.stFrameData.pVirAddr[0]);
        }
        else if(context->settings.md_en && context->status & FLAG_WIFI_CONNECTED){
            handle_motion_det(context, stBufInfo.stFrameData.pVirAddr[0], diff);
        }
        MI_SYS_ChnOutputPortPutBuf(bufHandle);
    }
    zscanner_exit(context->zscanner);
    motion_detect_exit(context->motion);
    return NULL;
}
static void run_wlan_map_check(CONTEXT *context)
{
    int i = 0;;
    int wlan0_up = 0;
    int write_wlan_map = 0;
    char buf[128];
    wlan0_up = check_wlan0_up();
    if (0 == wlan0_up)
    {
        printf("wlan0 up!!!\n");
        write_wlan_map = get_wlan_map_and_compare();
        if (write_wlan_map != 0) //reset wlan0 map
        {
            for (i = 0; i < 3; i++)
            {
                play_mp3_file(context, BEEP_AUDIO_FILE, 0);
                usleep(1000*1000);
            }
            set_wlan_map();
            printf("set wlan map over!!!\n\n\n");
            if (context->status & FLAG_GET_WLAN_INFO)
            {
                memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "wifi_disconnect.sh && wifi_off.sh");
                system(buf);
                memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "wifi_on.sh && wifi_connect.sh \"%s\" \"%s\" &", context->wifiap, context->passwd);
                system(buf);
            }
        }
        else
        {
            if (context->status & FLAG_GET_WLAN_INFO)
            {
                memset(buf, 0x00, sizeof(buf));
                snprintf(buf, sizeof(buf), "wifi_connect.sh \"%s\" \"%s\" &", context->wifiap, context->passwd);
                system(buf);
            }
        }
        context->status &=~ FLAG_CHECK_WLAN_MAP;
        printf("run_wlan_map_check over!!!\n\n\n");
    }
    else
    {
        printf("\n\nWait wlan0 up!!!\n\n");
    }
}

static void run_sdcard_check(CONTEXT *context)
{
    int sddet = !get_gpio(GPIO_SD_DET);
    if (sddet != !!(context->sdstatus->status & SDSTATUS_MOUNTED) || (context->sdstatus->status & SDSTATUS_REQ_FORMAT)) {
        if (context->sdstatus->status & SDSTATUS_REQ_FORMAT) {
            context->sdstatus->status &= ~SDSTATUS_REQ_FORMAT;
            context->sdstatus->status |=  SDSTATUS_FORMATTING;
        }
        if (sddet) context->sdstatus->status |= SDSTATUS_INSERTED;
        else       context->sdstatus->status &=~SDSTATUS_INSERTED;
        if (sddet == 0 || (context->sdstatus->status & SDSTATUS_FORMATTING)) {
            printf("sd ejected !\n");
            if (0 == umount2(SDCARD_MOUNT_PATH, MNT_FORCE|MNT_DETACH)) {
                printf("sd unmounted !\n");
                if (!(context->sdstatus->status & SDSTATUS_FORMATTING)) play_mp3_file(context, SDCARD_EJECT_FILE, 0);
            }
            context->sdstatus->status  &= sddet ? ~SDSTATUS_MOUNTED : 0;
            context->sdstatus->total    = 0;
            context->sdstatus->available= 0;
        }
        if (sddet == 1 || (context->sdstatus->status & SDSTATUS_FORMATTING)) {
            char *dev = NULL, buf[128];
            int   retry = 10;
            printf("sd inserted !\n");
            if (1) {
                if(context->status & FLAG_SD_FIRST_INSERT){
                    system("insmod /config/modules/4.9.84/mmc_core.ko");
                    system("insmod /config/modules/4.9.84/mmc_block.ko");
                    system("insmod /config/modules/4.9.84/kdrv_sdmmc.ko");
                    system("insmod /config/modules/4.9.84/fat.ko");
                    system("insmod /config/modules/4.9.84/msdos.ko");
                    system("insmod /config/modules/4.9.84/vfat.ko");
                    context->status &= ~FLAG_SD_FIRST_INSERT;
                    system("mkdir /tmp/sdcard");
                }
            }
            while (--retry) {
                if      (file_exist("/dev/mmcblk0p1")) dev = "/dev/mmcblk0p1";
                else if (file_exist("/dev/mmcblk0"  )) dev = "/dev/mmcblk0"  ;
                usleep(200*1000);
            }
            if (!dev) {
                printf("failed to find mmc block device !\n");
                return;
            }
            if (context->sdstatus->status & SDSTATUS_FORMATTING) {
                snprintf(buf, sizeof(buf), "mkfs.vfat %s", dev);
                if (system(buf) != 0) {
                    printf("mkfs.vfat ng !\n");
                } else {
                    printf("mkfs.vfat ok !\n");
                }
            }
            if (( mount(dev, SDCARD_MOUNT_PATH, "vfat" , MS_SYNCHRONOUS|MS_DIRSYNC, "fmask=022,dmask=022") == 0) \
                && get_sdcard_status(&context->sdstatus->status, &context->sdstatus->total, &context->sdstatus->available) ) {
                if (!(context->sdstatus->status & SDSTATUS_FORMATTING)) play_mp3_file(context, SDCARD_INSERT_FILE, 0);
                if (file_exist(SDCARD_MOUNT_PATH"/uuid/{084629C0-A7F7-4725-B242-44442CD5ABAB}")) {
                    int uid_update_result = sdcard_update_uid(SDCARD_MOUNT_PATH);
                    switch (uid_update_result){
                        case  0: play_mp3_file(context, SDCARD_UPDATE_UID_OK, 1); usleep(1000*1000);break;
                        case -1: printf("uuid update failed\n"); break;
                        case -2: printf("UUID exhaustion\n"); break;
                    }
                }
                if (file_exist(SDCARD_MOUNT_PATH"/update.a43727f8d6fe8a4ec28130ff26d73831")) {
                    play_mp3_file(context, FIRMWARE_UPDATING, 1); usleep(1000*1000);
                    system("tar xzf "SDCARD_MOUNT_PATH"/update.a43727f8d6fe8a4ec28130ff26d73831 -C /tmp");
                    system("rm /tmp/sdcard/update.a43727f8d6fe8a4ec28130ff26d73831");
                    system("/tmp/update/update.sh &");
                }else if (file_exist(SDCARD_MOUNT_PATH"/factorytest/ba8d528741f49cbe9cf07df72d113b88")) {
                    char buf[256], *temp;
                    char mode[16]   = "";
                    char ftip[16]   = "";
                    memset(context->wifiap, 0x00, sizeof(context->wifiap));
                    memset(context->passwd, 0x00, sizeof(context->passwd));
                    file_read(SDCARD_MOUNT_PATH"/factorytest/factorytest.ini", buf, sizeof(buf));
                    temp = strtok(buf , ":"); if (temp) strncpy(mode  , temp, sizeof(mode  ));
                    temp = strtok(NULL, ":"); if (temp) strncpy(ftip  , temp, sizeof(ftip  ));
                    temp = strtok(NULL, ":"); if (temp) strncpy(context->wifiap, temp, sizeof(context->wifiap));
                    temp = strtok(NULL, ":"); if (temp) strncpy(context->passwd, temp, sizeof(context->passwd));
                    if (strcmp(context->settings.ft_mode, mode) != 0) {
                        IPCAMSETTINGS newsettings = context->settings;
                        strncpy(newsettings.ft_mode, mode, sizeof(newsettings.ft_mode));
                        ipcam_settings_save(&newsettings, 1);
                        printf("ft_mode changed, reboot now\n");
                        sleep(3);
                        system("reboot -f");
                    }
                    if (*context->passwd == '\n') *context->passwd = '\0';
                    if (strcmp(context->wifiap, "") != 0 && strcmp(context->passwd, "") != 0) {
                        context->status |= FLAG_GET_WLAN_INFO;
                    }
                    if (strcmp(mode, "null") == 0) mode[0] = '\0';
                    if (strcmp(ftip, "null") == 0) ftip[0] = '\0';
                    if (strcmp(context->settings.ft_uid, ftip) != 0) {
                        IPCAMSETTINGS newsettings = context->settings;
                        strncpy(newsettings.ft_uid, ftip, sizeof(newsettings.ft_uid));
                        strncpy(context->settings.ft_uid, ftip, sizeof(context->settings.ft_uid));
                        ipcam_settings_save(&newsettings, 1);
                    }
                    if (strcmp(mode, "wfdp") == 0) {
                        system(SDCARD_MOUNT_PATH"/factorytest/wifi_test.sh");
                        play_mp3_file(context, WIFI_TEST_FILE, 1);
                    }
                }
                context->hwstate |= HW_SD_TEST_OK;
                printf("sd mounted !\n");
            }
        }
        context->sdstatus->status &= ~SDSTATUS_FORMATTING;
    }
}

void ipcam_request_idr(void)
{
    CONTEXT *context = (CONTEXT*)&g_app_ctx;
    context->status |= FLAG_REQUEST_IDR_MAIN;
}

static void sig_handler(int sig)
{
    CONTEXT *context  = &g_app_ctx;
    printf("sig_handler %d\n", sig);
    switch (sig) {
    case SIGINT:
    case SIGTERM:
        context->status |= FLAG_EXIT_VMAIN_THEADS|FLAG_EXIT_VSUB_THEADS|FLAG_EXIT_OTHER_THEADS;
        context->exit_tuya = 1;
        break;
    }
}

static void ipcam_apply_settings(CONTEXT *ctxt, IPCAMSETTINGS *newsettings)
{
    int paired_changed = ctxt->settings.paired != newsettings->paired;
    int stdby_changed  = ctxt->settings.standby != newsettings->standby;
    int flip_changed   = ctxt->settings.hflip_en != newsettings->hflip_en || ctxt->settings.vflip_en != newsettings->vflip_en;
    int ir_en_changed  = ctxt->settings.ir_en  != newsettings->ir_en ;
    int mdsen_changed  = ctxt->settings.md_sensitivity != newsettings->md_sensitivity;
    
    pthread_mutex_lock(&ctxt->lock1);
    ctxt->settings = *newsettings; // important!! update settings at the beginning to avoid thread safety problems
    
    if (ir_en_changed) {
        ctxt->last_motion_report = get_tick_count(); // to avoid motion report
        ctxt->last_irmode        = -1;
    }
    if (mdsen_changed) {
        motion_detect_sensitivity(ctxt->motion, newsettings->md_sensitivity);
    }
    if (paired_changed && !newsettings->paired) {
        ctxt->status |=  FLAG_ENABLE_ZSCANNER;
    }
    if (flip_changed) {
        ctxt->last_motion_report = get_tick_count(); // to avoid motion report
        MI_SNR_SetOrien(E_MI_SNR_PAD_ID_0, !newsettings->hflip_en, !newsettings->vflip_en);  //bMirror竖直翻转 bFlip水平翻转
    }

    if (stdby_changed) {
        ctxt->last_motion_report = get_tick_count(); // to avoid motion report
        if (newsettings->standby) {
            ctxt->status |= FLAG_EXIT_VMAIN_THEADS;
            if (ctxt->pthread_main != (pthread_t)NULL) { pthread_join(ctxt->pthread_main, NULL); ctxt->pthread_main = (pthread_t)NULL; }
            ctxt->status |= FLAG_EXIT_VSUB_THEADS;
            if (ctxt->pthread_sub  != (pthread_t)NULL) { pthread_join(ctxt->pthread_sub , NULL); ctxt->pthread_sub  = (pthread_t)NULL; }
            if (ctxt->pthread_scan != (pthread_t)NULL) { pthread_join(ctxt->pthread_scan, NULL); ctxt->pthread_scan = (pthread_t)NULL; }
            if (ctxt->pthread_jpeg != (pthread_t)NULL) { pthread_join(ctxt->pthread_jpeg, NULL); ctxt->pthread_jpeg = (pthread_t)NULL; }
        } else {
            ctxt->status &=~(FLAG_EXIT_VMAIN_THEADS|FLAG_EXIT_VSUB_THEADS|FLAG_VMAIN_INITED|FLAG_VSUB_INITED);
            if (ctxt->pthread_main == (pthread_t)NULL) pthread_create(&ctxt->pthread_main, NULL, main_stream        , ctxt);
            if (ctxt->pthread_sub  == (pthread_t)NULL) pthread_create(&ctxt->pthread_sub , NULL, sub_stream         , ctxt);
            if (ctxt->pthread_scan == (pthread_t)NULL) pthread_create(&ctxt->pthread_scan, NULL, scan_thread        , ctxt);
            if (ctxt->pthread_jpeg == (pthread_t)NULL) pthread_create(&ctxt->pthread_jpeg, NULL, jpeg_snapshot_proc , ctxt);
        }
    }
    pthread_mutex_unlock(&ctxt->lock1);
}

static void* ftest_and_rpc_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    struct sockaddr_in server = {};
    struct sockaddr_in client = {};
    socklen_t clientlen  = sizeof(client);
    struct    timeval tv = {1};
    int       sock       = -1;
    char      msg[512];
    int       len;

    
    pthread_setname_np(pthread_self(), "test");
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("failed to open factory test socket !\n");
        return NULL;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(FACTORY_TEST_PORT);
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("failed to bind factory test socket to port %d !\n", FACTORY_TEST_PORT);
        close(sock);
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv));
    while (!(context->status & FLAG_EXIT_OTHER_THEADS)) {
        len = recvfrom(sock, (void*)msg, sizeof(msg), 0, (struct sockaddr*)&client, &clientlen);
        if (len > 0) {
            msg[len < sizeof(msg) ? len : sizeof(msg) - 1] = '\0';
            if (strstr(msg, "rpc! ") == msg) {
                char *temp;
                int   id  ;
                temp = strtok(msg , " ");
                temp = strtok(NULL, " "); id = atoi(temp);
                temp = strtok(NULL, " ");
                if (strcmp(temp, "apply_settings") == 0) {
                    IPCAMSETTINGS settings;
                    ipcam_settings_load(&settings);
                    ipcam_apply_settings(context, &settings);
                } else if (strcmp(temp, "mp3") == 0) {
                    temp = strtok(NULL, " ");
                    play_mp3_file(context, temp, 0);
                } else if (strcmp(temp, "motor") == 0) {
                    temp = strtok(NULL, " ");
                    context->ptz_move = atoi(temp);
                    printf("context->ptz_move = %d\n", context->ptz_move);
                } else if (strcmp(temp, "reboot") == 0) {
                    play_mp3_file(context, BEEP_AUDIO_FILE, 0);
                    usleep(500*1000);
                    sync();system("reboot -f");
                }
                snprintf(msg, sizeof(msg), "rpc. %d", id);
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "uid?") == 0) {
                snprintf(msg, sizeof(msg), "uid:%s", context->devuid);
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "sid?") == 0) {
                snprintf(msg, sizeof(msg), "sid:%s", context->devsid);
                len = strlen(msg) + 1;
            }else if (strcmp(msg, "sn?") == 0) {
                snprintf(msg, sizeof(msg), "sn:%s", context->devsn);
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "micspktest!") == 0) {
                play_mp3_file(context, BEEP_AUDIO_FILE, 1);
                context->spkmic_test_tick = get_tick_count();
                snprintf(msg, sizeof(msg), "micspktest.");
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "result?") == 0) {
                snprintf(msg, sizeof(msg), "result:%c%c%c%c%c", !(context->hwstate & HW_MIC_TESTED) ? '-' : (context->hwstate & HW_MIC_TEST_OK) ? 'y' : 'n',
                    (context->hwstate & (HW_BTN_UP_OK |HW_BTN_DOWN_OK )) == (HW_BTN_UP_OK |HW_BTN_DOWN_OK ) ? 'y' : '-',
                    (context->hwstate & (HW_LSEN_UP_OK|HW_LSEN_DOWN_OK)) == (HW_LSEN_UP_OK|HW_LSEN_DOWN_OK) ? 'y' : '-',
                    (context->hwstate &  HW_SD_TEST_OK ) ? 'y' : 'n',
                    (context->hwstate &  HW_LIGHTCAM_OK) ? 'y' : 'n');
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "ver?") == 0) {
                snprintf(msg, sizeof(msg), "ver:%s-%d", FIRMWARE_VERSION, get_build_num());
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "mac?") == 0) {
                char mac[12];
                get_dev_mac(mac);
                snprintf(msg, sizeof(msg), "mac:%s", mac);
                len = strlen(msg) + 1;
            } else if (strcmp(msg, "ft_uid?") == 0) {
                snprintf(msg, sizeof(msg), "ft_uid:%s", context->settings.ft_uid);
                len = strlen(msg) + 1;
            } else if (strstr(msg, "uid=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                set_dev_ids(NULL, NULL, NULL, msg + 4, NULL);
                strncpy(context->devuid, msg + 4, sizeof(context->devuid));
                snprintf(msg, sizeof(msg), "uid.");
                len = strlen(msg) + 1;
            } else if (strstr(msg, "sid=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                set_dev_ids(NULL, NULL, msg + 4, NULL, NULL);
                strncpy(context->devsid, msg + 4, sizeof(context->devsid));
                snprintf(msg, sizeof(msg), "sid.");
                len = strlen(msg) + 1;
            } else if (strstr(msg, "sn=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                set_dev_ids(NULL, NULL, NULL, NULL, msg + 3);
                strncpy(context->devsn, msg + 3, sizeof(context->devsn));
                snprintf(msg, sizeof(msg), "sn.");
                len = strlen(msg) + 1;
            } else if (strstr(msg, "iperf3=") == msg) {
                set_iperf3_start(context, atoi(msg + 7));
                len = strlen(msg) + 1;
                snprintf(msg, sizeof(msg), "iperf3.");
            } else if (strstr(msg, "mac=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                set_wlan_mac(msg + 4);
            } else if (strstr(msg, "wifi_signal?") == msg) {
                snprintf(msg, sizeof(msg), "wifi_signal:%d", get_wifi_signal());
                len = strlen(msg) + 1;
            } else if (strstr(msg, "nLV=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.nSensor_LV = atoi(msg + 4);
            } else if (strstr(msg, "nSG=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.nSensor_Gain = atoi(msg + 4);
            } else if (strstr(msg, "nAWB=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.nSensor_AWB = (float)atof(msg + 5);
            }  else if (strstr(msg, "dLV=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.dSensor_LV = atoi(msg + 4);
            } else if (strstr(msg, "dSG=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.dSensor_Gain = atoi(msg + 4);
            } else if (strstr(msg, "ir=") == msg && strcmp(context->settings.ft_mode, "smt") == 0) {
                context->settings.ir_en = atoi(msg + 3);
            }
            sendto(sock, msg, len, 0, (struct sockaddr*)&client, clientlen);
        } 
    }
    close(sock);
    return NULL;
}

int get_isp_data(CONTEXT *context)
{
    MI_S32 s32Ret = MI_ISP_OK;
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t    isp_init;  //isp初始化返回值
    MI_ISP_AE_EXPO_INFO_TYPE_t         pExpInfo;  //当前整体曝光信息
    MI_ISP_AWB_HW_STATISTICS_TYPE_t  pAWBHWdata;  //AWB 硬件统计值
    uint8_t *pstart = NULL;
    uint8_t *pend   = NULL;
    uint32_t rsum = 0, gbsum = 0;
    float    fawb;
    
    if( MI_ISP_IQ_GetParaInitStatus(0,&isp_init) != MI_ISP_OK){
        return -1;
    }
    if(!isp_init.stParaAPI.bFlag){
        return -1;
    } 
    if(context->status & FLAG_VMAIN_INITED && context->status & FLAG_WIFI_CONNECTED){
        s32Ret = MI_ISP_AWB_GetHWStats(0, &pAWBHWdata);
        pstart = pAWBHWdata.u8AwbBuffer;
        pend   = pAWBHWdata.u8AwbBuffer + sizeof(pAWBHWdata.u8AwbBuffer);
        if (MI_ISP_OK == s32Ret){
            while (pstart < pend) {
                rsum   += pstart[0];
                gbsum  += pstart[1] + pstart[2];
                pstart += 3;
            }
            fawb = (float)gbsum / (float)rsum;
            context->settings.soft_light_sensor_AWB = fawb;
        } else{
            printf("MI_ISP_AWB_GetHWStats failed!\n");
        }
        s32Ret = MI_ISP_AE_QueryExposureInfo(0,&pExpInfo);
        if (MI_ISP_OK == s32Ret){
            context->settings.soft_light_sensor_LV = pExpInfo.u32LVx10;
            context->settings.soft_light_sensor_SensorGain = pExpInfo.stExpoValueLong.u32SensorGain;
        } else{
            printf("MI_ISP_AE_QueryExposureInfo failed!\n");
        }
    } else {
        return 0;
    }
}
int soft_light_sensor(CONTEXT *context)
{
    MI_S32 s32Ret = MI_ISP_OK;
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t    isp_init;  //isp初始化返回值
    int cur_irmode = context->last_irmode;

    if( MI_ISP_IQ_GetParaInitStatus(0,&isp_init) != MI_ISP_OK){
        return -1;
    }
    if(!isp_init.stParaAPI.bFlag){
        return -1;
    } 
    if (context->settings.ir_en == 1) {
        cur_irmode = 1;  //day mode
    }
    else if(context->settings.ir_en == 2){
        cur_irmode = 0;  //night mode
    }
    else if(context->settings.ir_en == 0){
        if(context->last_irmode == 1) { //day to night
            if ((context->settings.soft_light_sensor_LV <= context->settings.dSensor_LV) && (context->settings.soft_light_sensor_SensorGain >= context->settings.dSensor_Gain)) {
                sleep(5);
                if ((context->settings.soft_light_sensor_LV <= context->settings.dSensor_LV) && (context->settings.soft_light_sensor_SensorGain >= context->settings.dSensor_Gain)) {
                    cur_irmode = 0;
                    context->hwstate |= HW_LSEN_DOWN_OK;
                }
            }
        } else if (context->last_irmode == 0) { // night to day
            if ((context->settings.soft_light_sensor_LV >= context->settings.nSensor_LV) && (context->settings.soft_light_sensor_SensorGain <= context->settings.nSensor_Gain) && (context->settings.soft_light_sensor_AWB >= context->settings.nSensor_AWB)) {
                sleep(5);
                if ((context->settings.soft_light_sensor_LV >= context->settings.nSensor_LV) && (context->settings.soft_light_sensor_SensorGain <= context->settings.nSensor_Gain) && (context->settings.soft_light_sensor_AWB >= context->settings.nSensor_AWB)) {
                    cur_irmode = 1;
                    context->hwstate |= HW_LSEN_UP_OK;
                }
            }
        } else {
            cur_irmode = 1;    
        }
    }      
    if (context->last_irmode != cur_irmode) {
        MI_ISP_IQ_COLORTOGRAY_TYPE_t enflag;
        enflag.bEnable = !cur_irmode;  //设定彩转灰功能的布尔值
        context->last_motion_report = get_tick_count(); // to avoid motion report
        MI_ISP_IQ_SetColorToGray(0, &enflag);
        if (!cur_irmode) usleep(500*1000);
        MI_ISP_API_CmdLoadBinFile(0, cur_irmode ? context->ispbinday: context->ispbinight, 1234);
        set_ircut(!cur_irmode);
        set_gpio(GPIO_IR_LED, context->settings.ir_en == 1?0:context->settings.ir_en == 2?1:!cur_irmode);
        context->last_irmode = cur_irmode;
        printf("cur_irmode = %d\n", cur_irmode);
    }
    return 0;
}

static void handle_spk_pwroff(CONTEXT *context)
{
    #define OFF_SPK_DELAY  1000
    if (context->playstoptick > 1 && get_tick_count() > context->playstoptick + OFF_SPK_DELAY) {
        printf("turn off speaker power !\n");
        set_gpio(GPIO_SPK_PWR, 0);
        context->playstoptick = 0;
    }
}

static void handle_button(CONTEXT *context, int *button_counter)
{
    IPCAMSETTINGS settings;
    int factorytest = strcmp(context->settings.ft_mode, "") != 0;
    if (get_gpio(GPIO_BUTTON) == 0) {
        *button_counter  += 1;
        context->hwstate |= HW_BTN_PRESSED;
        context->hwstate |= HW_BTN_DOWN_OK;
    } else {
        *button_counter   = 0;
        context->hwstate &=~HW_BTN_PRESSED;
        context->hwstate |= HW_BTN_UP_OK;
    }
    if (*button_counter == 5) {
        if (factorytest == 1) {
            context->hwstate &= ~HW_MIC_TEST_OK;
            play_mp3_file(context, BEEP_AUDIO_FILE,0);
            context->spkmic_test_tick = get_tick_count();
        }
    } else if (*button_counter == 15&& strcmp(context->settings.ft_mode, "smt") != 0) {
        if (factorytest == 0) {
            settings = context->settings;
            settings.standby = 0;
            settings.paired  = 0;
            settings.led_en  = 1;
            ipcam_settings_save(&settings, 1);
            play_mp3_file(context, BEEP_AUDIO_FILE, 0);
            unlink(WIFI_CONF_FILE);
            unlink(TUYA_DB_FILE1 );
            unlink(TUYA_DB_FILE2 );
            usleep(500*1000);
            sync(); system("reboot -f");
        }
    } else if (*button_counter == 50) {
        play_mp3_file(context, BEEP_AUDIO_FILE, 0);
        unlink(SETTINGS_FILE );
        unlink(WIFI_CONF_FILE);
        usleep(500*1000);
        sync(); system("reboot -f");
    }
}

static void* ptz_move_control(void *argv){
    CONTEXT *context = (CONTEXT*)argv;
    int  thread_counter = 0;
    int  ptz_move       = 0;

    context->ptz_move = -1;
    while (!(context->status & FLAG_EXIT_OTHER_THEADS)) {

        if (context->ptz_move >= 0) {
            thread_counter += 1;
            ptz_move = context->ptz_move;
        }

        if (context->ptz_move == -1) {
            motor_speed(context->motor, 10, 10);
            switch (ptz_move) {
            case 1: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 500); break;
            case 5: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-500); break;
            case 3: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 500); break;
            case 7: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-500); break;
            }
            context->ptz_move = -2;
        } else if (30 <= thread_counter && thread_counter < 60 && 0 <= context->ptz_move) {
            motor_speed(context->motor, 10, 10);
            switch (ptz_move) {
            case 1: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 500); break;
            case 5: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-500); break;
            case 3: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 500); break;
            case 7: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-500); break;
            }
        } else if (60 <= thread_counter && 0 <= context->ptz_move) {
            motor_speed(context->motor, 5, 10);
            switch (ptz_move) {
            case 1: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 1000); break;
            case 5: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-1000); break;
            case 3: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 1000); break;
            case 7: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-1000); break;
            }
        }
        if (context->ptz_move == -1) {
            thread_counter = 0;
            context->ptz_move = -2;
        }
        usleep(50*1000);
    }
    return NULL;
}

void run_aging_test(CONTEXT *context, int threadcounter)
{
    if (strcmp(context->settings.ft_mode, "aging") == 0 || strcmp(context->settings.ft_mode, "mtest") == 0) {
        int sec = threadcounter / 10;
        if (sec == 2) play_mp3_file(context, AGING_TEST_FILE, 1);
        if (sec > 25) {
            context->sdstatus->status = 0; usleep(100*1000); // wait recording stopped
            if (strcmp(context->settings.ft_mode, "mtest") != 0) system("sync && reboot -f");
        }
        if (sec % 10 == 0) {
            set_gpio(GPIO_IR_LED, !((sec / 10) & 1));
            set_ircut(0);
            set_ircut(1);
        }
    }
}

void run_motor_test(CONTEXT *context, int threadcounter, char *mode)
{
    int sec   =  threadcounter  / 10;
    int turn  = (threadcounter  / 10) % 36;
    if (threadcounter == 0 && (strcmp(context->settings.ft_mode, "monly") == 0 || strcmp(context->settings.ft_mode, "mtest") == 0)) {
        motor_speed(context->motor, 5, 11);
    }
    if (threadcounter % 200 == 0 && strcmp(mode, "mtest") == 0) { // 20s
        play_mp3_file(context, BEEP_AUDIO_FILE, 1);
    }
    if (strcmp(mode, "smt") == 0) {
        switch (sec) {
        case  0: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 3000); break;
        case  3: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-3000); break;
        case  6: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 3000); break;
        case  9: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-3000); break;
        case 12: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 0   ); break;
        }
    } else if (strcmp(mode, "aging") == 0) {
        switch (sec) {
        case  0: case 13: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 3000); break;
        case  3: case 16: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-3000); break;
        case  6: case 19: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 3000); break;
        case  9: case 22: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-3000); break;
        case 12: case 25: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 0   ); break;
        }
    } else if (strcmp(mode, "monly") == 0 || strcmp(mode, "mtest") == 0) { // move from left to right 30000, move from up to down 15000
        switch (turn) {
        case  0: motor_action(context->motor, MOTOR_TYPE_MOTOR1, 10000); break;
        case 10: motor_action(context->motor, MOTOR_TYPE_MOTOR1,-10000); break;
        case 20: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 6000 ); break;
        case 26: motor_action(context->motor, MOTOR_TYPE_MOTOR2,-8000 ); break;
        case 34: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 2000 ); break;
        case 36: motor_action(context->motor, MOTOR_TYPE_MOTOR2, 0    ); break;
        }
    }
}

static void* run_led_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    int threadcounter = 0;
    int savecounter   = 0;
    int lastleds      =(1 << 2);
    int cur_leds      = 0;
    int start_count   = 0;
    int led_mode      = 0;
    int changes;
    pthread_setname_np(pthread_self(), "led");
    while(!(context->status & FLAG_EXIT_OTHER_THEADS))
    {
        if (strcmp(context->settings.ft_mode, "") != 0) {
            if (!(context->hwstate & HW_BTN_PRESSED)) {
                led_mode = 4;
            }
            else{
                led_mode = 0;
            }
        }else {
            if (context->settings.led_en && !context->settings.standby) {
                if (context->status & FLAG_ENABLE_ZSCANNER) {  //扫码状态
                    led_mode = 2;
                } else if (AUTO_CONNECT) {  //根据上次保存的wifi信息自动连接
                    led_mode = 3;
                    if(threadcounter/10 > 120)//超过2分钟还未连接成功
                    {
                        led_mode = 0;
                    }
                } else if (!(context->status & (FLAG_WIFI_CONNECTED)) && !(context->status & FLAG_ENABLE_ZSCANNER)) {  //扫码连接
                    if(!start_count){
                        start_count = 1;
                        savecounter = threadcounter;
                    }
                    led_mode = 3;
                    if((threadcounter - savecounter)/10 > 180)  //扫码成功但连接不上持续3分钟以上
                    {
                        led_mode = 0;
                    }
                }
                if ((context->status & (FLAG_WIFI_CONNECTED)) && (context->settings.paired)) {  //设备成功连接
                    led_mode = 1;
                    start_count = 0;
                }
                if(REMOVE_DEVICE){  //移除设备
                    led_mode = 0;
                }
            }
        }
        switch(led_mode){
            case 0:  //红灯常亮
                cur_leds |= 1 << 2;
                break;
            case 1:  //绿灯常亮
                cur_leds |= 1 << 1;
                break;
            case 2:  //红灯闪烁（间隔1s）
                cur_leds = ((threadcounter / 10) % 2 == 0) << 2;
                break;
            case 3:  //绿灯闪烁（间隔1s）
                cur_leds = ((threadcounter / 10) % 2 == 0) << 1;
                break;
            case 4:  //红绿灯交互闪烁（间隔1s）
                cur_leds = ((threadcounter / 10) % 2 == 0) << 2;
                cur_leds = ((threadcounter / 10) % 2 == 1) << 1;
                break;
        }
        threadcounter++;
        changes = lastleds ^ cur_leds;
        if (changes & (1 << 3)) set_led(GPIO_LED_U, !!(cur_leds & (1 << 3)));
        if (changes & (1 << 2)) set_led(GPIO_LED_R, !!(cur_leds & (1 << 2)));
        if (changes & (1 << 1)) set_led(GPIO_LED_G, !!(cur_leds & (1 << 1)));
        lastleds = cur_leds;
        usleep(100*1000);
    }
    return NULL;
}

static void run_leds(CONTEXT *context, int threadcounter, int *lastleds)
{
    int cur_leds = 0;
    int changes;

    if (strcmp(context->settings.ft_mode, "") != 0) {
        if (!(context->hwstate & HW_BTN_PRESSED)) {
            cur_leds |= ((threadcounter / 10) % 3 == 0) << 2;
            cur_leds |= ((threadcounter / 10) % 3 == 1) << 1;
            cur_leds |= ((threadcounter / 10) % 3 == 2) << 0;
        }
    } else if (!context->settings.standby && context->settings.led_en) {
        switch (context->ledtype) {
        case 0: cur_leds |= (1 << 2); break;
        case 1: cur_leds |= ((threadcounter / 2 ) & 1) << 2; break;
        case 2: cur_leds |= ((threadcounter / 16) & 1) << 2; break;
        }
    }

    changes = *lastleds ^ cur_leds;
    if (changes & (1 << 3)) set_led(GPIO_LED_U, !!(cur_leds & (1 << 3)));
    if (changes & (1 << 2)) set_led(GPIO_LED_R, !!(cur_leds & (1 << 2)));
    if (changes & (1 << 1)) set_led(GPIO_LED_G, !!(cur_leds & (1 << 1)));
    *lastleds = cur_leds;
}
static void* device_monitor_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    int  thread_counter = 0;
    int  button_counter = 0;
    int  last_leds      =(1 << 2);

    snprintf(context->ispbinday , sizeof(context->ispbinday ), "/customer/res/gc2053_day.bin");
    snprintf(context->ispbinight, sizeof(context->ispbinight), "/customer/res/gc2053_night.bin");
    pthread_setname_np(pthread_self(), "dmon"); 
    context->status |= FLAG_CHECK_WLAN_MAP;
    context->status &=~ FLAG_GET_WLAN_INFO;
    context->settings.nSensor_LV = 25;
    context->settings.nSensor_Gain = 10000;
    context->settings.nSensor_AWB = 1.81;
    context->settings.dSensor_LV = 1;
    context->settings.dSensor_Gain = 50000;
    while (!(context->status & FLAG_EXIT_OTHER_THEADS)) {
        if (thread_counter % 10 == 0) { // 1s
            get_isp_data(context);
            soft_light_sensor(context);
            run_sdcard_check (context);
            run_aging_test   (context, thread_counter);
            run_motor_test   (context, thread_counter, context->settings.ft_mode);
            handle_spk_pwroff(context);     
        }
#if 0
        if(thread_counter % 20 == 0) { //2s
            if (context->status & FLAG_CHECK_WLAN_MAP){
                run_wlan_map_check(context);
            }   
        }
#endif    
        if(thread_counter % 60 == 0){ //6s
            ipcam_settings_save(&context->settings, 1);
        }
        if (thread_counter % 300 == 0) { // 30s
            ipcam_settings_save(NULL, 1);// flush settings to file
            if ((context->sdstatus->status & SDSTATUS_MOUNTED) && context->settings.rec_type) {
                get_sdcard_status(NULL, &context->sdstatus->total, &context->sdstatus->available);    
            }
        }
        run_leds(context, thread_counter, &last_leds);
        handle_button(context, &button_counter);
        thread_counter += 1; // increase counter
        usleep(100*1000);
    }
    return NULL;
}

static void mic_auto_test_run(CONTEXT *context, int16_t *buf, int len)
{
    #define FFT_POINT_NUM  512
    float  fsamples[FFT_POINT_NUM * 2];
    float *fdst, maxamp = 0;
    int    freqidx = 0;
    int    testdone= 0;
    int    n, i;

    if (context->spkmic_test_tick) {
        if (!context->fft) context->fft = fft_init(FFT_POINT_NUM);
        n = MIN(FFT_POINT_NUM, len/sizeof(int16_t));
        for (i=0,fdst=fsamples; i<n; i++) {
            *fdst++ = *buf++;
            *fdst++ =  0;
        }
        fft_execute(context->fft, fsamples, fsamples);
        for (i=0; i<n/2; i++) {
            float curamp = sqrt(fsamples[i*2+0]*fsamples[i*2+0] + fsamples[i*2+1]*fsamples[i*2+1]);
            if (maxamp < curamp) {
                maxamp = curamp;
                freqidx= i;
            }
        }
        printf("freqidx = %d, maxamp = %.2f\n", freqidx, maxamp);
         if ((freqidx == 64 || freqidx == 192) && maxamp > 500000) {
            play_mp3_file(context, MICOK_AUDIO_FILE, 0);
            context->hwstate |= HW_MIC_TEST_OK;
            testdone = 1;
        }
        if (get_tick_count() - context->spkmic_test_tick > 1000) {
            if (!(context->hwstate & HW_MIC_TEST_OK)) {
                play_mp3_file(context, MICNG_AUDIO_FILE, 0);
            }
            testdone = 1;
        }
        if (testdone) {
            context->hwstate |= HW_MIC_TESTED;
            context->spkmic_test_tick = 0;
            fft_free(context->fft);
            context->fft = NULL;
        }
    }
}

static void mic_auto_test_exit(CONTEXT *context)
{
    fft_free(context->fft);
    context->fft = NULL;
}

#ifdef WAV_TEST
int addWaveHeader(WaveFileHeader_t* tWavHead, AencType_e eAencType,int raw_len)
{
    tWavHead->chRIFF[0] = 'R';
    tWavHead->chRIFF[1] = 'I';
    tWavHead->chRIFF[2] = 'F';
    tWavHead->chRIFF[3] = 'F';

    tWavHead->chWAVE[0] = 'W';
    tWavHead->chWAVE[1] = 'A';
    tWavHead->chWAVE[2] = 'V';
    tWavHead->chWAVE[3] = 'E';

    tWavHead->chFMT[0] = 'f';
    tWavHead->chFMT[1] = 'm';
    tWavHead->chFMT[2] = 't';
    tWavHead->chFMT[3] = 0x20;
    tWavHead->dwFMTLen = 0x10;

    if(eAencType == E_AENC_TYPE_G711A)
    {
        tWavHead->wave.wFormatTag = 0x06;
    }

    if(eAencType == E_AENC_TYPE_G711U)
    {
        tWavHead->wave.wFormatTag = 0x07;
    }

    if(eAencType == E_AENC_TYPE_G711U || eAencType == E_AENC_TYPE_G711A)
    {

        tWavHead->wave.wChannels = 0x01;
        tWavHead->wave.wBitsPerSample = 8;//bitWidth;g711encode出来是8bit，这里需要写死
        tWavHead->wave.dwSamplesPerSec = 8000;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.wChannels) / 8;
    }
    else if(eAencType == PCM)
    {

        tWavHead->wave.wChannels = 0x01;
        tWavHead->wave.wFormatTag = 0x1;
        tWavHead->wave.wBitsPerSample = 16; //16bit
        tWavHead->wave.dwSamplesPerSec = 8000;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = 1024;
    }
    else //g726
    {
        tWavHead->wave.wChannels = 0x01;
        tWavHead->wave.wFormatTag = 0x45;
        switch(eAencType)
        {
            case E_AENC_TYPE_G726_40:
                tWavHead->wave.wBitsPerSample = 5;
                tWavHead->wave.wBlockAlign =  5;
                break;
            case E_AENC_TYPE_G726_32:
                tWavHead->wave.wBitsPerSample = 4;
                tWavHead->wave.wBlockAlign =  4;
                break;
            case E_AENC_TYPE_G726_24:
                tWavHead->wave.wBitsPerSample = 3;
                tWavHead->wave.wBlockAlign =  3;
                break;
            case E_AENC_TYPE_G726_16:
                tWavHead->wave.wBitsPerSample = 2;
                tWavHead->wave.wBlockAlign =  2;
                break;
            default:
                printf("eAencType error:%d\n", eAencType);
                return -1;
        }

        tWavHead->wave.dwSamplesPerSec = 8000;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
    }

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(WaveFileHeader_t) - 8;

    return 0;
}
#endif

static void* audio_capture_proc(void *argv)
{
    CONTEXT     *context = (CONTEXT*)argv;
    MI_AUDIO_Frame_t frame   = {};
    uint16_t      buffer_pcm[512];
    int i;
    int ftest = strcmp(context->settings.ft_mode, "");
#ifdef WAV_TEST
    WaveFileHeader_t    stAiWavHead;
    int  fd,s32Ret;
    int total_size = 0;
#endif
    pthread_setname_np(pthread_self(), "audc");
#ifdef WAV_TEST
    fd = open((const char *)OUTPUTFILENAME, O_RDWR | O_CREAT, 0777);
    if(fd < 0){
        printf("Open output file path:%s fail \n", OUTPUTFILENAME);
        printf("error:%s", strerror(errno));
        return NULL;
    }
    memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
    s32Ret = write(fd, &stAiWavHead, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        printf("write wav head failed\n");
        return NULL;
    }
#endif
    while (!(context->status & FLAG_EXIT_OTHER_THEADS)) {
        if(context->status & FLAG_WIFI_CONNECTED)
        {   
            if (context->settings.standby) { usleep(100*1000); continue; }
            if (MI_SUCCESS == MI_AI_GetFrame(AI_DEV_ID, AI_CHN_ID0, &frame, NULL, -1)) {
                if (ftest) avkcps_audio(context->avkcps, frame.apVirAddr[0], frame.u32Len, 1);
                for (i=0; i<frame.u32Len; i++) buffer_pcm[i] = alaw2pcm(((unsigned char *)frame.apVirAddr[0])[i]);   
                mic_auto_test_run(context, (int16_t*)buffer_pcm, frame.u32Len*2); // mic test
                if(!ftest) tuya_audio(frame.apVirAddr[0], frame.u32Len);
                MI_AI_ReleaseFrame(AI_DEV_ID, AI_CHN_ID0, &frame, NULL);
            } else {
                printf("MI_AI_GetFrame failed !\n");
                usleep(100*1000); continue;
            }
        }
        else
        {
            usleep(100*1000);continue;
        }  
    }
#ifdef WAV_TEST
    memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
    addWaveHeader(&stAiWavHead,E_AENC_TYPE_G711A,total_size);
    lseek(fd, 0, SEEK_SET);
    s32Ret = write(fd, &stAiWavHead, sizeof(WaveFileHeader_t));
    if(s32Ret < 0)
    {
        printf("write aiwavhead failed!\n");
    }
    close(fd);
#endif
    mic_auto_test_exit(context);
    return NULL;
}

static void* network_monitor_proc(void *argv)
{
    CONTEXT *context = (CONTEXT*)argv;
    char wlanip[16]  = "";
    static int network_sec_count;
    int ftest = strcmp(context->settings.ft_mode, "") != 0;
    pthread_setname_np(pthread_self(),"nmon");
    while (!(context->status & FLAG_EXIT_OTHER_THEADS)) {
        if (context->settings.paired) { // 判断设备是否成功配对过
            context->status |= FLAG_HAVE_PAIRED;
        }
        if (get_dev_ip("wlan0", wlanip, sizeof(wlanip)) == 0){
            GET_IP_FLAG = 1;
            context->status |= FLAG_WIFI_CONNECTED;
            if (!ftest && get_mqtt_status() == 0 && (context->status & FLAG_HAVE_PAIRED) && network_sec_count % 5 == 0) // mqtt 未连接且设备已经成功配对过一次（每5秒判断一次）
            {
                system("kill `ps | awk '$5==\"udhcpc\" && $7==\"wlan0\" {printf $1}'`");
                system("udhcpc -i wlan0 -T 1 &");
            }
        } else {
            context->status &=~ FLAG_WIFI_CONNECTED;
        }
        usleep(1000*1000); // 每一秒刷新一次网络状态
        network_sec_count ++;
    }
    return NULL;    
}

int main(int argc, char *argv[])
{
    CONTEXT      *context  = &g_app_ctx;
    IPCAMSETTINGS settings;
    int           shmid;
    int           ftest, pid;

    pid = check_proc_running(argv[0]);
    if (pid > 0) {
        printf("ipcam is running...\n");
        if (argc >= 2) send_msg_to_ipcam(argv[1], argc >= 3 ? argv[2] : NULL);
        return 0;
    } else {
        ipcam_settings_load(&settings);
        ftest = strcmp(settings.ft_mode, "") != 0;
    }
    apkapi_init();
    set_led(GPIO_LED_R, 1);
    set_led(GPIO_LED_G, 0);
    
    system("ifconfig lo up");
    system("wifi_on.sh &");
    system("echo 1 > /proc/sys/vm/overcommit_memory");

    shmid = shmget((key_t)SDSTATUS_SHMID, sizeof(SDSTATUS), 0666|IPC_CREAT);
    ST_DefaultArgs(&context->pstConfig);
    context->status |= FLAG_SD_FIRST_INSERT;
    if (shmid != -1) {
        context->sdstatus = (SDSTATUS*)shmat(shmid, NULL, 0);
        if (context->sdstatus == (SDSTATUS*)-1) {
            context->sdstatus = NULL;
        }
    } else {
        printf("shmget failed !\n");
    }

    if (strcmp(settings.ft_mode, "smt") == 0 || strcmp(settings.ft_mode, "rtsp") == 0) {
        settings.main_venc_type = 1;
        ipcam_settings_save(&settings, 0);
    }
 
    STCHECKRESULT(ST_BaseModuleInit(&context->pstConfig));  //MI的各模块初始化，要放在wavein_init之前
    waveout_init();
    wavein_init(ftest);
    context->mp3dec = mp3dec_init();
    if (!ftest && pid == -1) {
        play_mp3_file(context, WELCOME_AUDIO_FILE, 0); // pid -1 mean \safely start, -2 mean ipcam crashed then restart
    }
 
    if (ftest) {
        context->avkcps = avkcps_init(8000, "alaw", 1, 8000, "h264", 1920, 1080, 15, 256, ipcam_request_idr);
    }

    signal(SIGINT , sig_handler);
    signal(SIGTERM, sig_handler);
    //context->motor = motor_init();
   
    get_dev_uid(context->devuid, sizeof(context->devuid));
    get_dev_sid(context->devsid, sizeof(context->devsid));
    get_dev_sn (context->devsn , sizeof(context->devsn ));
    printf("\n\nver:%s-%d\n\n", FIRMWARE_VERSION, get_build_num());
    // init pthread attr
    pthread_attr_init(&context->pthread_attr);
    pthread_attr_setstacksize(&context->pthread_attr, 128 * 1024);
    // apply settings
    context->settings.standby = context->settings.mic_en   =  context->last_irmode = -1;
    context->settings.ir_en = 0;
    context->settings.paired  = context->settings.hflip_en = context->settings.md_en = context->settings.light_mode = -1;
    ipcam_apply_settings(context, &settings);

    pthread_create(&context->pthread_dmon, &context->pthread_attr, device_monitor_proc   , context);
    pthread_create(&context->pthread_led , &context->pthread_attr, run_led_proc          , context);
    pthread_create(&context->pthread_nmon, &context->pthread_attr, network_monitor_proc  , context);
    pthread_create(&context->pthread_test, &context->pthread_attr, ftest_and_rpc_proc    , context);
    pthread_create(&context->pthread_rgn , &context->pthread_attr, UpdateRgnOsdTimeProc  , context);
    pthread_create(&context->pthread_audc, &context->pthread_attr, audio_capture_proc    , context);
    //pthread_create(&context->pthread_ptzm, &context->pthread_attr, ptz_move_control      , context);
    if(!ftest) tuya_ipc_main(context->devuid, context->devsid, NULL,&(context->exit_tuya));
    if (context->pthread_led ) pthread_join(context->pthread_led , NULL);
    if (context->pthread_dmon) pthread_join(context->pthread_dmon, NULL);
    if (context->pthread_nmon) pthread_join(context->pthread_nmon, NULL);
    if (context->pthread_scan) pthread_join(context->pthread_scan, NULL);
    if (context->pthread_sub ) pthread_join(context->pthread_sub , NULL);
    if (context->pthread_main) pthread_join(context->pthread_main, NULL);
    if (context->pthread_jpeg) pthread_join(context->pthread_jpeg, NULL);
    if (context->pthread_test) pthread_join(context->pthread_test, NULL);
    if (context->pthread_rgn)  pthread_join(context->pthread_rgn , NULL);
    if (context->pthread_audc) pthread_join(context->pthread_audc, NULL);
    if (context->pthread_ptzm) pthread_join(context->pthread_ptzm, NULL);
    
    while (context->pthread_mp3) usleep(200*1000); // wait mp3 thread exit
    avkcps_exit(context->avkcps);
    waveout_exit();
    wavein_exit();
    /*mp3 decode exit*/
    mp3dec_exit(context->mp3dec);
    motor_exit(context->motor);
    STCHECKRESULT(ST_BaseModuleUnInit());
    system("wifi_disconnect.sh && wifi_off.sh &");
    set_led (GPIO_LED_R , 0);
    set_led (GPIO_LED_G , 0);
    set_gpio(GPIO_IR_LED, 0);
    if (context->sdstatus) shmdt(context->sdstatus);
    exit_proc_running(argv[0]);
    printf("ipcam safe exit.\n");
    return 0;
}

void ipcam_set_ledtype(int type)
{
    CONTEXT *context = (CONTEXT*)&g_app_ctx;
    context->ledtype = type;
}

int ipcam_take_snapshot(void **buf, int *size)
{
    CONTEXT *context = (CONTEXT*)&g_app_ctx;
    int      timeout = 10;
    context->snapshot_len = 0;
    context->status |= FLAG_JPEG_SNAPSHOT;
    while (--timeout && context->snapshot_len == 0) usleep(100*1000);
    *buf  = timeout > 0 ? context->snapshot_buf : NULL;
    *size = timeout > 0 ? context->snapshot_len : 0;
    return timeout > 0 ? 0 : -1;
}

#ifdef WAV_TEST
int create_wavfile(char *buf, int len)
{
    WaveFileHeader_t    stAiWavHead;
    int  fd,s32Ret;
    int total_size = 0;
    int write_size = 0;

    fd = open((const char *)OUTPUTFILENAME, O_RDWR | O_CREAT, 0777);
    if(fd < 0){
        printf("Open output file path:%s fail \n", OUTPUTFILENAME);
        printf("error:%s", strerror(errno));
        return 1;
    }
    memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
    s32Ret = write(fd, &stAiWavHead, sizeof(WaveFileHeader_t));
    if (s32Ret < 0)
    {
        printf("write wav head failed\n");
        return 1;
    }
    while(total_size <= 256*1024){
        write_size = write(fd,buf,len);
        total_size+=write_size;
        printf("total_size = %d\n",total_size);
    }
    memset(&stAiWavHead, 0, sizeof(WaveFileHeader_t));
    addWaveHeader(&stAiWavHead,E_AENC_TYPE_G711U,total_size);
    lseek(fd, 0, SEEK_SET);
    s32Ret = write(fd, &stAiWavHead, sizeof(WaveFileHeader_t));
    if(s32Ret < 0)
    {
        printf("write aiwavhead failed!\n");
    }
    close(fd);
    usleep(4*1000*1000);
    return 0;
}
#endif

void ipcam_play_audio(void *buf, int len)
{
    CONTEXT *context = (CONTEXT*)&g_app_ctx;
    if (!(context->status & FLAG_MP3_ISPLAYING)) {
        if (context->playstoptick == 0) set_gpio(GPIO_SPK_PWR, 1);
        context->status |=  FLAG_VOICE_ISPLAYING;
        waveout_buffer(buf, len, 2);
        context->status &= ~FLAG_VOICE_ISPLAYING;
        context->playstoptick = get_tick_count();
    }

}
