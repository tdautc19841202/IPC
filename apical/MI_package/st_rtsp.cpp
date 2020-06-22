/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <vector>
#include <string>
#include "st_common.h"
#include "st_vif.h"
#include "st_vpe.h"
#include "st_venc.h"
#include "st_rgn.h"
#include "st_ao.h"
#include "dot_matrix_font.h"
#include "mi_rgn.h"
#include "mi_divp.h"
#include "mi_ao.h"
#include "mi_aio_datatype.h"
#include "st_rtsp.h"

MI_U32 g_u32CapWidth = 0;
MI_U32 g_u32CapHeight = 0;
static ST_RGN_Osd_T g_stRgnOsd;
ST_Config_S g_stConfig;
ST_VDF_OSD_Info_T g_stVDFOsdInfo[MAX_FULL_RGN_NULL];
MI_BOOL bHaftRing = FALSE;
static MI_U32 g_md_detect_cnt_bak[MAX_FULL_RGN_NULL];
static ST_Rect_T g_stRect_Bak[(RAW_W / 4) * (RAW_H / 4)][MAX_FULL_RGN_NULL] = {{0,},};
class CanvasScopeLock
{
    public:
        explicit CanvasScopeLock()
        {
            pthread_mutex_lock(&_gstCanvasMutex);
        }
        ~CanvasScopeLock()
        {
            pthread_mutex_unlock(&_gstCanvasMutex);
        }
        static void CanvasLock()
        {
            pthread_mutex_lock(&_gstCanvasMutex);
        }
        static void CanvasUnlock()
        {
            pthread_mutex_unlock(&_gstCanvasMutex);
        }
    private:
        static pthread_mutex_t _gstCanvasMutex;
};
pthread_mutex_t CanvasScopeLock::_gstCanvasMutex = PTHREAD_MUTEX_INITIALIZER;

#define CANVAS_LOCK CanvasScopeLock::CanvasLock()
#define CANVAS_UNLOCK CanvasScopeLock::CanvasUnlock()

void canvas_lock(void)
{
    CanvasScopeLock::CanvasLock();
}

void canvas_unlock(void)
{
    CanvasScopeLock::CanvasUnlock();
}

void ST_CaptureJPGProc(MI_VENC_CHN VencChn)
{
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack;
    MI_U32 u32BypassCnt = 1;

    s32Ret = MI_VENC_StartRecvPic(VencChn);

    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("ST_Venc_StartChannel fail, 0x%x\n", s32Ret);
        return;
    }

    memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
    memset(&stPack, 0, sizeof(MI_VENC_Pack_t));
    stStream.pstPack = &stPack;
    stStream.u32PackCount = 1;

    while(1)
    {
        s32Ret = MI_VENC_GetStream(VencChn, &stStream, 40);
        if(MI_SUCCESS == s32Ret)
        {
            if (0 == u32BypassCnt)
            {
                printf("##########Start to write file!!!#####################\n");
                //ST_SaveStreamToFile(&stStream);
                printf("##########End to write file!!!#####################\n");
            }
            else
            {
                printf("Bypasss frame, because region osd attach action not currect!\n");
            }
            s32Ret = MI_VENC_ReleaseStream(VencChn, &stStream);
            if(MI_SUCCESS != s32Ret)
            {
                ST_DBG("MI_VENC_ReleaseStream fail, ret:0x%x\n", s32Ret);
            }

            ST_DBG("u32BypassCnt:%d\n", u32BypassCnt);

            if (0 == u32BypassCnt)
                break;

            u32BypassCnt--;
        }
        else
            printf("Continue!!!\n");

        usleep(200 * 1000);
    }

    s32Ret = MI_VENC_StopRecvPic(VencChn);

    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("ST_Venc_StopChannel fail, 0x%x\n", s32Ret);
    }
}
MI_S32 ST_StartPipeLine(MI_U8 i, MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32CropW, MI_U32 u32CropH, MI_U32 u32CropX, MI_U32 u32CropY)
{
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    int arraySize = ARRAY_SIZE(g_stStreamAttr);
    ST_Sys_BindInfo_T stBindInfo;
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_U32 u32DevId = -1;
    MI_SYS_WindowRect_t stRect;
    MI_VENC_CHN VencChn = 0;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_RGN_Attr_t stRgnAttr;
    MI_RGN_ChnPort_t stChnPort;
    MI_VENC_RcParam_t stRcParam;
    MI_RGN_ChnPortParam_t stChnPortParam;
    ST_Config_S *pstConfig = &g_stConfig;
    ST_VDF_OSD_Info_T *pstVDFOsdInfo = g_stVDFOsdInfo;
    MI_VENC_InputSourceConfig_t stVenInSrc;

    CanvasScopeLock ScopeLock;
    if(i >= arraySize)
    {
        printf("index is out of bounds!\n");
        return MI_FAIL;
    }
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = pstStreamAttr[i].u32InputChn;
    stVpePortInfo.u16OutputWidth = u32Width;
    stVpePortInfo.u16OutputHeight = u32Height;
    printf("Vpe create port w %d h %d\n", u32Width, u32Height);

    if (pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_JPEGE &&
        pstStreamAttr[i].eBindType == E_MI_SYS_BIND_TYPE_REALTIME)
    {
        stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
    }
    else
    {
        stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    }

    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    if (u32CropW != 0 && u32CropH != 0)
    {
        stRect.u16Width = u32CropW;
        stRect.u16Height = u32CropH;
        stRect.u16X = u32CropX;
        stRect.u16Y = u32CropY;
        ExecFunc(MI_VPE_SetPortCrop(pstStreamAttr[i].u32InputChn, pstStreamAttr[i].u32InputPort, &stRect), MI_SUCCESS);
    }
    STCHECKRESULT(ST_Vpe_StartPort(pstStreamAttr[i].u32InputPort, &stVpePortInfo));

    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H264E)
    {
        stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
        stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;
#if 0
        //stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264FIXQP;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateNum = 30;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateDen = 1;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32Gop = 30;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32IQp = 25;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32PQp = 25;
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
#endif
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264VBR;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxQp = 44;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MinQp = 24;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32Gop = pstStreamAttr[i].u32FrameRate * 2;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32StatTime = 0;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateNum = pstStreamAttr[i].u32FrameRate;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateDen = 1;

        stRcParam.stParamH264VBR.s32IPQPDelta = 3;
        stRcParam.stParamH264VBR.u32MaxIQp = 44;
        stRcParam.stParamH264VBR.u32MinIQp = 24;
        stRcParam.stParamH264VBR.u32MaxIPProp = 44;
        stRcParam.stParamH264VBR.s32ChangePos = 100;

    }
    else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H265E)
    {
        stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH265e.bByFrame = TRUE;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265CBR;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateNum = pstStreamAttr[i].u32FrameRate;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32Gop = pstStreamAttr[i].u32FrameRate * 2;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32StatTime = 0;
    }
    else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_JPEGE)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth =  u32Width;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth =  u32Width;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrJpeg.bByFrame = TRUE;
    }
    stChnAttr.stVeAttr.eType = pstStreamAttr[i].eType;
    VencChn = pstStreamAttr[i].vencChn;
    STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));
    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);
        VencChn = pstStreamAttr[i].vencChn;
    memset(&stVenInSrc, 0, sizeof(MI_VENC_InputSourceConfig_t));
    if (pstStreamAttr[i].eBindType == E_MI_SYS_BIND_TYPE_HW_RING)
    {
        if (bHaftRing)
        {
            stVenInSrc.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_RING_HALF_FRM;
            MI_VENC_SetInputSourceConfig(VencChn, &stVenInSrc);
        }
        else
        {
            stVenInSrc.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_RING_ONE_FRM;
            MI_VENC_SetInputSourceConfig(VencChn, &stVenInSrc);
        }
    }
    else if (pstStreamAttr[i].eBindType == E_MI_SYS_BIND_TYPE_FRAME_BASE)
    {
        stVenInSrc.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_NORMAL_FRMBASE;
        MI_VENC_SetInputSourceConfig(VencChn, &stVenInSrc);
    }
    if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H264E)STCHECKRESULT(MI_VENC_SetRcParam(VencChn, &stRcParam));
    STCHECKRESULT(ST_Venc_StartChannel(VencChn));

    // attach
    memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stChnPort.eModId = E_MI_RGN_MODID_VPE;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
    stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    //stPoint of time_osd   zyh_add
    stChnPortParam.stPoint.u32X = 10;
    stChnPortParam.stPoint.u32Y = 10;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = 100;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 0;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 0;
    stChnPortParam.unPara.stCoverChnPort.u32Color = 0;
    ExecFunc(MI_RGN_AttachToChn(RGN_OSD_HANDLE, &stChnPort, &stChnPortParam), MI_RGN_OK);
#if 0
    // create cover1
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[i].u32Cover1Handle, &stRgnAttr), MI_RGN_OK);
    stChnPort.eModId = E_MI_RGN_MODID_VPE;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
    stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 100;
    stChnPortParam.stPoint.u32Y = 100;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[i].u32Cover1Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 255, 0);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[i].u32Cover1Handle, &stChnPort, &stChnPortParam), MI_RGN_OK);
    // create cover2
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[i].u32Cover2Handle, &stRgnAttr), MI_RGN_OK);
    stChnPort.eModId = E_MI_RGN_MODID_VPE;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
    stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 400;
    stChnPortParam.stPoint.u32Y = 400;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[i].u32Cover2Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 0, 255);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[i].u32Cover2Handle, &stChnPort, &stChnPortParam), MI_RGN_OK);
#endif
    if (pstConfig->s32UseVdf)
    {
        if (i < MAX_FULL_RGN_NULL)
        {
            pstVDFOsdInfo[i].hHandle = RGN_FOR_VDF_BEGIN + i;
            pstVDFOsdInfo[i].eModId = E_MI_RGN_MODID_VPE;
            pstVDFOsdInfo[i].u32Chn = pstStreamAttr[i].u32InputChn;
            pstVDFOsdInfo[i].u32Port = pstStreamAttr[i].u32InputPort;

            memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
            stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
            stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
            stRgnAttr.stOsdInitParam.stSize.u32Width = u32Width;
            stRgnAttr.stOsdInitParam.stSize.u32Height = u32Height;
            ExecFunc(ST_OSD_Create(pstVDFOsdInfo[i].hHandle, &stRgnAttr), MI_RGN_OK);
            memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
            stChnPort.eModId = E_MI_RGN_MODID_VPE;
            stChnPort.s32DevId = 0;
            stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
            stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
            memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
            stChnPortParam.bShow = TRUE;
            stChnPortParam.stPoint.u32X = 0;
            stChnPortParam.stPoint.u32Y = 0;
            stChnPortParam.unPara.stOsdChnPort.u32Layer = 0;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
            ExecFunc(MI_RGN_AttachToChn(pstVDFOsdInfo[i].hHandle, &stChnPort, &stChnPortParam), MI_RGN_OK);
        }
    }

    // bind VPE->VENC
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr[i].u32InputChn;
    stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[i].u32InputPort;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32DevId = u32DevId;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.eBindType = pstStreamAttr[i].eBindType;
    if (stBindInfo.eBindType == E_MI_SYS_BIND_TYPE_HW_RING)
    {
        stBindInfo.u32BindParam = bHaftRing ? u32Height/2 : u32Height;
    }
    else
    {
        stBindInfo.u32BindParam = 0;
    }
    printf("Bind type %d, para %d\n", stBindInfo.eBindType, stBindInfo.u32BindParam);
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    return MI_SUCCESS;
}

MI_S32 ST_StartPipeLineWithDip(MI_U8 i, MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32CropW, MI_U32 u32CropH, MI_U32 u32CropX, MI_U32 u32CropY)
{
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    int arraySize = ARRAY_SIZE(g_stStreamAttr);
    ST_Sys_BindInfo_T stBindInfo;
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_U32 u32DevId = -1;
    MI_SYS_WindowRect_t stRect;
    MI_VENC_CHN VencChn = 0;
    MI_VENC_RcParam_t stRcParam;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_DIVP_ChnAttr_t stDivpChnAttr;

    MI_RGN_Attr_t stRgnAttr;
    MI_RGN_ChnPort_t stChnPort;
    MI_RGN_ChnPortParam_t stChnPortParam;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    MI_VENC_InputSourceConfig_t stVenInSrc;

    ST_Config_S *pstConfig = &g_stConfig;
    ST_VDF_OSD_Info_T *pstVDFOsdInfo = g_stVDFOsdInfo;

    CanvasScopeLock ScopeLock;
    if(i >= arraySize)
    {
        printf("index is out of bounds!\n");
        return MI_FAIL;
    }
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = pstStreamAttr[i].u32InputChn;
    stVpePortInfo.u16OutputWidth = u32Width;
    stVpePortInfo.u16OutputHeight = u32Height;
    printf("Vpe create port w %d h %d\n", u32Width, u32Height);
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    if (u32CropW != 0 && u32CropH != 0)
    {
        stRect.u16Width = u32CropW;
        stRect.u16Height = u32CropH;
        stRect.u16X = u32CropX;
        stRect.u16Y = u32CropY;
        ExecFunc(MI_VPE_SetPortCrop(pstStreamAttr[i].u32InputChn, pstStreamAttr[i].u32InputPort, &stRect), MI_SUCCESS);
    }
    STCHECKRESULT(ST_Vpe_StartPort(pstStreamAttr[i].u32InputPort, &stVpePortInfo));
    memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    stDivpChnAttr.bHorMirror            = FALSE;
    stDivpChnAttr.bVerMirror            = FALSE;
    stDivpChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType           = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X       = 0;
    stDivpChnAttr.stCropRect.u16Y       = 0;
    stDivpChnAttr.stCropRect.u16Width   = 0;
    stDivpChnAttr.stCropRect.u16Height  = 0;
    stDivpChnAttr.u32MaxWidth           = u32Width;
    stDivpChnAttr.u32MaxHeight          = u32Height;
    ExecFunc(MI_DIVP_CreateChn(DIVP_CHN_FOR_SCALE + i, &stDivpChnAttr), MI_SUCCESS);
    ExecFunc(MI_DIVP_StartChn(DIVP_CHN_FOR_SCALE + i), MI_SUCCESS);
    memset(&stOutputPortAttr, 0, sizeof(stOutputPortAttr));
    stOutputPortAttr.eCompMode          = E_MI_SYS_COMPRESS_MODE_NONE;
    stOutputPortAttr.ePixelFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stOutputPortAttr.u32Width           = u32Width;
    stOutputPortAttr.u32Height          = u32Height;
    STCHECKRESULT(MI_DIVP_SetOutputPortAttr(DIVP_CHN_FOR_SCALE + i, &stOutputPortAttr));
    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H264E)
    {
        stChnAttr.stVeAttr.stAttrH264e.u32PicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH264e.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH264e.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH264e.u32BFrameNum = 2;
        stChnAttr.stVeAttr.stAttrH264e.bByFrame = TRUE;
#if 0
        //stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264FIXQP;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateNum = 30;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRateDen = 1;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32Gop = 30;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32IQp = 25;
        //stChnAttr.stRcAttr.stAttrH264FixQp.u32PQp = 25;
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264CBR;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32Gop = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateNum = 30;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH264Cbr.u32StatTime = 0;
#endif
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H264VBR;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MaxQp = 48;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32MinQp = 28;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32Gop = pstStreamAttr[i].u32FrameRate * 2;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32StatTime = 0;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateNum = pstStreamAttr[i].u32FrameRate;
        stChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRateDen = 1;

        stRcParam.stParamH264VBR.s32IPQPDelta = 0;
        stRcParam.stParamH264VBR.u32MaxIQp = 48;
        stRcParam.stParamH264VBR.u32MinIQp = 28;
        stRcParam.stParamH264VBR.u32MaxIPProp = 50;
        stRcParam.stParamH264VBR.s32ChangePos = 100;

    }
    else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H265E)
    {
        stChnAttr.stVeAttr.stAttrH265e.u32PicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH265e.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrH265e.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrH265e.bByFrame = TRUE;

        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_H265CBR;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = pstStreamAttr[i].f32Mbps * 1024 * 1024;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateNum = pstStreamAttr[i].u32FrameRate;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRateDen = 1;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32Gop = pstStreamAttr[i].u32FrameRate * 2;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32FluctuateLevel = 0;
        stChnAttr.stRcAttr.stAttrH265Cbr.u32StatTime = 0;
    }
    else if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_JPEGE)
    {
        stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
        stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = u32Width;
        stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32Height;
        stChnAttr.stVeAttr.stAttrJpeg.bByFrame = TRUE;
    }
    stChnAttr.stVeAttr.eType = pstStreamAttr[i].eType;
    VencChn = pstStreamAttr[i].vencChn;
    STCHECKRESULT(ST_Venc_CreateChannel(VencChn, &stChnAttr));
    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);
    VencChn = pstStreamAttr[i].vencChn;
    memset(&stVenInSrc, 0, sizeof(MI_VENC_InputSourceConfig_t));
    stVenInSrc.eInputSrcBufferMode = E_MI_VENC_INPUT_MODE_NORMAL_FRMBASE;
    MI_VENC_SetInputSourceConfig(VencChn, &stVenInSrc);
    if(pstStreamAttr[i].eType == E_MI_VENC_MODTYPE_H264E)STCHECKRESULT(MI_VENC_SetRcParam(VencChn, &stRcParam));
    STCHECKRESULT(ST_Venc_StartChannel(VencChn));

    // attach
    memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
    stChnPort.s32OutputPortId = 0;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 2;
    stChnPortParam.stPoint.u32Y = 2;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
    ExecFunc(MI_RGN_AttachToChn(RGN_OSD_HANDLE1, &stChnPort, &stChnPortParam), MI_RGN_OK);
#if 0
    // create cover1
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[i].u32Cover1Handle, &stRgnAttr), MI_RGN_OK);
    stChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
    stChnPort.s32OutputPortId = 0;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 100;
    stChnPortParam.stPoint.u32Y = 100;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[i].u32Cover1Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 255, 0);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[i].u32Cover1Handle, &stChnPort, &stChnPortParam), MI_RGN_OK);
    // create cover2
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[i].u32Cover2Handle, &stRgnAttr), MI_RGN_OK);
    stChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
    stChnPort.s32OutputPortId = 0;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 400;
    stChnPortParam.stPoint.u32Y = 400;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[i].u32Cover2Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 0, 255);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[i].u32Cover2Handle, &stChnPort, &stChnPortParam), MI_RGN_OK);
#endif
    if (pstConfig->s32UseVdf)
    {
        if (i < MAX_FULL_RGN_NULL)
        {
            pstVDFOsdInfo[i].hHandle = RGN_FOR_VDF_BEGIN + i;
            pstVDFOsdInfo[i].eModId = E_MI_RGN_MODID_DIVP;
            pstVDFOsdInfo[i].u32Chn = DIVP_CHN_FOR_SCALE + i;
            pstVDFOsdInfo[i].u32Port = 0;

            memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
            stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
            stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
            stRgnAttr.stOsdInitParam.stSize.u32Width = u32Width;
            stRgnAttr.stOsdInitParam.stSize.u32Height = u32Height;
            ExecFunc(ST_OSD_Create(pstVDFOsdInfo[i].hHandle, &stRgnAttr), MI_RGN_OK);

            memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
            stChnPort.eModId = E_MI_RGN_MODID_DIVP;
            stChnPort.s32DevId = 0;
            stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
            stChnPort.s32OutputPortId = 0;
            memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
            stChnPortParam.bShow = TRUE;
            stChnPortParam.stPoint.u32X = 0;
            stChnPortParam.stPoint.u32Y = 0;
            stChnPortParam.unPara.stOsdChnPort.u32Layer = 0;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
            stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
            ExecFunc(MI_RGN_AttachToChn(pstVDFOsdInfo[i].hHandle, &stChnPort, &stChnPortParam), MI_RGN_OK);
        }
    }

    // bind VPE to divp
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr[i].u32InputChn;
    stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[i].u32InputPort;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_FOR_SCALE + i;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    // bind divp to venc
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = DIVP_CHN_FOR_SCALE + i;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32DevId = u32DevId;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    return MI_SUCCESS;

}
MI_S32 ST_StopPipeLine(MI_U8 i)
{
    MI_VENC_CHN VencChn = 0;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    ST_Sys_BindInfo_T stBindInfo;
    MI_U32 u32DevId = -1;
    MI_RGN_ChnPort_t stChnPort;
    int arraySize = ARRAY_SIZE(g_stStreamAttr);

    ST_Config_S *pstConfig = &g_stConfig;
    ST_VDF_OSD_Info_T *pstVDFOsdInfo = g_stVDFOsdInfo;

    CanvasScopeLock ScopeLock;
    if(i >= arraySize)
    {
        printf("index is out of bounds!\n");
        return MI_FAIL;
    }
    // destory cover
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[i].u32Cover1Handle), MI_RGN_OK);
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[i].u32Cover2Handle), MI_RGN_OK);
    if (pstStreamAttr[i].u32InputPort != 2)
    {
        // Destory osd
        memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
        stChnPort.eModId = E_MI_RGN_MODID_VPE;
        stChnPort.s32DevId = 0;
        stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
        stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
        ExecFunc(MI_RGN_DetachFromChn(RGN_OSD_HANDLE, &stChnPort), MI_RGN_OK);
    }
    if (pstConfig->s32UseVdf)
    {
        if (i < MAX_FULL_RGN_NULL)
        {
            MI_U32 j = 0;

            stChnPort.eModId = E_MI_RGN_MODID_VPE;
            stChnPort.s32DevId = 0;
            stChnPort.s32ChnId = pstStreamAttr[i].u32InputChn;
            stChnPort.s32OutputPortId = pstStreamAttr[i].u32InputPort;
            ExecFunc(MI_RGN_DetachFromChn(pstVDFOsdInfo[i].hHandle, &stChnPort), MI_RGN_OK);
            ExecFunc(ST_OSD_Destroy(pstVDFOsdInfo[i].hHandle), MI_RGN_OK);
            pstVDFOsdInfo[i].hHandle = ST_OSD_HANDLE_INVALID;
            for(j = 0; j < g_md_detect_cnt_bak[i] && j < (RAW_W / 4) * (RAW_H / 4); j++)
            {
                memset(&g_stRect_Bak[j][i], 0, sizeof(ST_Rect_T));
            }
        }
    }

    VencChn = pstStreamAttr[i].vencChn;
    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr[i].u32InputChn;
    stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[i].u32InputPort;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32DevId = u32DevId;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    STCHECKRESULT(ST_Venc_StopChannel(VencChn));
    STCHECKRESULT(ST_Venc_DestoryChannel(VencChn));
    STCHECKRESULT(ST_Vpe_StopPort(pstStreamAttr[i].u32InputChn, pstStreamAttr[i].u32InputPort));

    return MI_SUCCESS;
}
MI_S32 ST_StopPipeLineWithDip(MI_U8 i)
{
    MI_VENC_CHN VencChn = 0;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    ST_Sys_BindInfo_T stBindInfo;
    MI_U32 u32DevId = -1;
    MI_RGN_ChnPort_t stChnPort;
    int arraySize = ARRAY_SIZE(g_stStreamAttr);
    ST_Config_S *pstConfig = &g_stConfig;
    ST_VDF_OSD_Info_T *pstVDFOsdInfo = g_stVDFOsdInfo;

    CanvasScopeLock ScopeLock;
    if(i >= arraySize)
    {
        printf("index is out of bounds!\n");
        return MI_FAIL;
    }
    // destory cover
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[i].u32Cover1Handle), MI_RGN_OK);
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[i].u32Cover2Handle), MI_RGN_OK);
    // Destory osd
    memset(&stChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stChnPort.s32DevId = 0;
    stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
    stChnPort.s32OutputPortId = 0;
    ExecFunc(MI_RGN_DetachFromChn(RGN_OSD_HANDLE, &stChnPort), MI_RGN_OK);

    if (pstConfig->s32UseVdf)
    {
        if (i < MAX_FULL_RGN_NULL)
        {
            MI_U32 j = 0;

            stChnPort.eModId = E_MI_RGN_MODID_DIVP;
            stChnPort.s32DevId = 0;
            stChnPort.s32ChnId = DIVP_CHN_FOR_SCALE + i;
            stChnPort.s32OutputPortId = 0;
            ExecFunc(MI_RGN_DetachFromChn(pstVDFOsdInfo[i].hHandle, &stChnPort), MI_RGN_OK);
            ExecFunc(ST_OSD_Destroy(pstVDFOsdInfo[i].hHandle), MI_RGN_OK);
            pstVDFOsdInfo[i].hHandle = ST_OSD_HANDLE_INVALID;
            for(j = 0; j < g_md_detect_cnt_bak[i] && j < (RAW_W / 4) * (RAW_H / 4); j++)
            {
                memset(&g_stRect_Bak[j][i], 0, sizeof(ST_Rect_T));
            }
        }
    }

    VencChn = pstStreamAttr[i].vencChn;
    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = DIVP_CHN_FOR_SCALE + i;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32DevId = u32DevId;
    stBindInfo.stDstChnPort.u32ChnId = VencChn;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    STCHECKRESULT(ST_Venc_StopChannel(VencChn));
    STCHECKRESULT(ST_Venc_DestoryChannel(VencChn));
    ExecFunc(MI_DIVP_StopChn(DIVP_CHN_FOR_SCALE + i), MI_SUCCESS);

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstStreamAttr[i].u32InputChn;
    stBindInfo.stSrcChnPort.u32PortId = pstStreamAttr[i].u32InputPort;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_FOR_SCALE + i;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    ExecFunc(MI_DIVP_DestroyChn(DIVP_CHN_FOR_SCALE + i), MI_SUCCESS);
    STCHECKRESULT(ST_Vpe_StopPort(pstStreamAttr[i].u32InputChn, pstStreamAttr[i].u32InputPort));

    return MI_SUCCESS;
}

MI_S32 ST_BaseModuleInit(ST_Config_S* pstConfig)
{
    MI_U32 u32CapWidth = 0, u32CapHeight = 0;
    MI_VIF_FrameRate_e eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    MI_SYS_PixelFormat_e ePixFormat;
    MI_RGN_Attr_t stRgnAttr;
    ST_VPE_ChannelInfo_T stVpeChannelInfo;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_VIF_HDRType_e eVifHdrType = E_MI_VIF_HDR_TYPE_OFF;
    MI_VPE_HDRType_e eVpeHdrType = E_MI_VPE_HDR_TYPE_OFF;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_SNR_Res_t stRes;
    MI_U32 u32ChocieRes =0;

    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));

    /************************************************
    Step1:  init SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());

    if(pstConfig->s32HDRtype > 0)
        MI_SNR_SetPlaneMode(E_MI_SNR_PAD_ID_0, TRUE);
    else
        MI_SNR_SetPlaneMode(E_MI_SNR_PAD_ID_0, FALSE);

    MI_SNR_QueryResCount(E_MI_SNR_PAD_ID_0, &u32ResCount);
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        MI_SNR_GetRes(E_MI_SNR_PAD_ID_0, u8ResIndex, &stRes);
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }

    printf("choice which resolution use, cnt %d\n", u32ResCount);
    do
    {
        u32ChocieRes = 0;
        MI_SNR_QueryResCount(E_MI_SNR_PAD_ID_0, &u32ResCount);
        if(u32ChocieRes >= u32ResCount)
        {
            printf("choice err res %d > =cnt %d\n", u32ChocieRes, u32ResCount);
        }
    }while(u32ChocieRes >= u32ResCount);

    printf("You select %d res\n", u32ChocieRes);

    MI_SNR_SetRes(E_MI_SNR_PAD_ID_0,u32ChocieRes);
    MI_SNR_SetFps(E_MI_SNR_PAD_ID_0, 15);
    MI_SNR_Enable(E_MI_SNR_PAD_ID_0);

    MI_SNR_GetPadInfo(E_MI_SNR_PAD_ID_0, &stPad0Info);
    MI_SNR_GetPlaneInfo(E_MI_SNR_PAD_ID_0, 0, &stSnrPlane0Info);

    g_u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    g_u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    u32CapWidth = stSnrPlane0Info.stCapRect.u16Width;
    u32CapHeight = stSnrPlane0Info.stCapRect.u16Height;
    eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);

    //g_stStreamAttr[0].u32Width = u32CapWidth;
    //g_stStreamAttr[0].u32Height = u32CapHeight;

    /************************************************
    Step2:  init VIF(for IPC, only one dev)
    *************************************************/
    eVifHdrType = (MI_VIF_HDRType_e)pstConfig->s32HDRtype;

    STCHECKRESULT(ST_Vif_EnableDev(0,eVifHdrType, &stPad0Info));

    ST_VIF_PortInfo_T stVifPortInfoInfo;
    memset(&stVifPortInfoInfo, 0, sizeof(ST_VIF_PortInfo_T));
    stVifPortInfoInfo.u32RectX = 0;
    stVifPortInfoInfo.u32RectY = 0;
    stVifPortInfoInfo.u32RectWidth = u32CapWidth;
    stVifPortInfoInfo.u32RectHeight = u32CapHeight;
    stVifPortInfoInfo.u32DestWidth = u32CapWidth;
    stVifPortInfoInfo.u32DestHeight = u32CapHeight;
    stVifPortInfoInfo.eFrameRate = eFrameRate;
    stVifPortInfoInfo.ePixFormat = ePixFormat;
    STCHECKRESULT(ST_Vif_CreatePort(0, 0, &stVifPortInfoInfo));
    STCHECKRESULT(ST_Vif_StartPort(0, 0, 0));
    //if (enRotation != E_MI_SYS_ROTATE_NONE)
    {
        MI_BOOL bMirror = FALSE, bFlip = FALSE;
        //ExecFunc(MI_VPE_SetChannelRotation(0, enRotation), MI_SUCCESS);

        switch(pstConfig->enRotation)
        {
        case E_MI_SYS_ROTATE_NONE:
            bMirror= FALSE;
            bFlip = FALSE;
            break;
        case E_MI_SYS_ROTATE_90:
            bMirror = FALSE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_180:
            bMirror = TRUE;
            bFlip = TRUE;
            break;
        case E_MI_SYS_ROTATE_270:
            bMirror = TRUE;
            bFlip = FALSE;
            break;
        default:
            break;
        }

        MI_SNR_SetOrien(E_MI_SNR_PAD_ID_0, bMirror, bFlip);
        MI_VPE_SetChannelRotation(0, pstConfig->enRotation);
    }
    memset(&stVpeChannelInfo, 0, sizeof(ST_VPE_ChannelInfo_T));
    eVpeHdrType = (MI_VPE_HDRType_e)pstConfig->s32HDRtype;
    stVpeChannelInfo.u16VpeMaxW = u32CapWidth;
    stVpeChannelInfo.u16VpeMaxH = u32CapHeight;
    stVpeChannelInfo.u32X = 0;
    stVpeChannelInfo.u32Y = 0;
    stVpeChannelInfo.u16VpeCropW = 0;
    stVpeChannelInfo.u16VpeCropH = 0;
    stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
    stVpeChannelInfo.eFormat = ePixFormat;
    stVpeChannelInfo.e3DNRLevel = pstConfig->en3dNrLevel;
    stVpeChannelInfo.eHDRtype = eVpeHdrType;
    stVpeChannelInfo.bRotation = FALSE;
    STCHECKRESULT(ST_Vpe_CreateChannel(0, &stVpeChannelInfo));
    STCHECKRESULT(ST_Vpe_StartChannel(0));
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
    //stBindInfo.u32SrcFrmrate = 15;
    //stBindInfo.u32DstFrmrate = 15;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    STCHECKRESULT(ST_OSD_Init());
    InitRGN();
    //ST_DoCaptureJPGProc(704, 396, E_MI_SYS_ROTATE_NONE);
    //MI_IQSERVER_Open(u32CapWidth, u32CapHeight, 0);
#if 0
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
    stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
    stRgnAttr.stOsdInitParam.stSize.u32Width = RGN_OSD_TIME_WIDTH;
    stRgnAttr.stOsdInitParam.stSize.u32Height = RGN_OSD_TIME_HEIGHT;
    ExecFunc(ST_OSD_Create(RGN_OSD_HANDLE, &stRgnAttr), MI_RGN_OK);
#endif
    return MI_SUCCESS;
}

MI_S32 InitRGN(void)
{
    printf("[ZYH_DEBUG]--->   InitRGN start!!!\n\n\n");
    MI_RGN_Attr_t stRgnAttr;
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
    stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
    stRgnAttr.stOsdInitParam.stSize.u32Width = RGN_OSD_TIME_WIDTH;
    stRgnAttr.stOsdInitParam.stSize.u32Height = RGN_OSD_TIME_HEIGHT;
    ExecFunc(ST_OSD_Create(RGN_OSD_HANDLE, &stRgnAttr), MI_RGN_OK);
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
    stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
    stRgnAttr.stOsdInitParam.stSize.u32Width = RGN_OSD_TIME_WIDTH;
    stRgnAttr.stOsdInitParam.stSize.u32Height = RGN_OSD_TIME_HEIGHT;
    ExecFunc(ST_OSD_Create(RGN_OSD_HANDLE1, &stRgnAttr), MI_RGN_OK);
#if 0    
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_OSD;
    stRgnAttr.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
    stRgnAttr.stOsdInitParam.stSize.u32Width = RGN_OSD_TIME_WIDTH;
    stRgnAttr.stOsdInitParam.stSize.u32Height = RGN_OSD_TIME_HEIGHT;
    ExecFunc(ST_OSD_Create(RGN_OSD_HANDLE2, &stRgnAttr), MI_RGN_OK);
#endif    
    return MI_SUCCESS;
}

MI_S32 ST_BaseModuleUnInit(void)
{
    ST_Sys_BindInfo_T stBindInfo;

    ExecFunc(ST_OSD_Destroy(RGN_OSD_HANDLE), MI_RGN_OK);
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;

    //stBindInfo.u32SrcFrmrate = pstStreamAttr[i].u32FrameRate;
    //stBindInfo.u32DstFrmrate = pstStreamAttr[i].u32FrameRate;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    /************************************************
    Step1:  destory VPE
    *************************************************/
    STCHECKRESULT(ST_Vpe_StopChannel(0));
    STCHECKRESULT(ST_Vpe_DestroyChannel(0));

    /************************************************
    Step2:  destory VIF
    *************************************************/
    STCHECKRESULT(ST_Vif_StopPort(0, 0));
    STCHECKRESULT(ST_Vif_DisableDev(0));

    /************************************************
    Step3:  destory SYS
    *************************************************/
    STCHECKRESULT(ST_Sys_Exit());

    //MI_IQSERVER_Close();

    return MI_SUCCESS;
}

void ST_DefaultConfig(ST_Config_S *pstConfig)
{
    pstConfig->s32UseOnvif     = 0;
    pstConfig->s32UseVdf    = 0;
#if USE_AUDIO
    pstConfig->s32UseAudio    = 0;
#endif
    pstConfig->s32LoadIQ    = 0;
#if (SHOW_PANEL == 1)
    pstConfig->s32UsePanel    = 0;
    pstConfig->enPixelFormat= E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
    pstConfig->s32Rotate    = 0;
#endif
    pstConfig->s32HDRtype    = 0;
    pstConfig->enSensorType = ST_Sensor_Type_IMX291;
    pstConfig->enRotation = E_MI_SYS_ROTATE_NONE;
}

void ST_DefaultArgs(ST_Config_S *pstConfig)
{
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_U32 i = 0, j = 0;

    memset(pstConfig, 0, sizeof(ST_Config_S));
    ST_DefaultConfig(pstConfig);

    pstConfig->s32UseOnvif = 0;
    pstConfig->s32UseVdf = 0;
    pstConfig->s32LoadIQ = 0;
    pstConfig->enRotation = E_MI_SYS_ROTATE_NONE;
    pstConfig->en3dNrLevel = E_MI_VPE_3DNR_LEVEL1;

    pstStreamAttr[0].bEnable = FALSE;

    pstStreamAttr[1].bEnable = TRUE;
    pstStreamAttr[1].eType = E_MI_VENC_MODTYPE_H264E;
    pstStreamAttr[1].u32Width = 1920;
    pstStreamAttr[1].u32Height = 1080;
    pstStreamAttr[1].eBindType = E_MI_SYS_BIND_TYPE_HW_RING;
    pstStreamAttr[1].u32BindPara = 1080;

    pstStreamAttr[2].bEnable = TRUE;
    pstStreamAttr[2].eType = E_MI_VENC_MODTYPE_H264E;
    pstStreamAttr[2].u32Width = 704;
    pstStreamAttr[2].u32Height = 396;
    pstStreamAttr[2].eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    pstStreamAttr[2].u32BindPara = 0;
    pstStreamAttr[2].enInput = ST_Sys_Input_DIVP;

    for(i = 0; i < MAX_CHN_NEED_OSD; i ++)
    {
        for(j = 0; j < MAX_OSD_NUM_PER_CHN; j ++)
        {
            g_stRgnOsd.stOsdInfo[i][j].hHandle = ST_OSD_HANDLE_INVALID;
        }
    }

    for (i = 0; i < MAX_FULL_RGN_NULL; i ++)
    {
        g_stVDFOsdInfo[i].hHandle = ST_OSD_HANDLE_INVALID;
    }
}

MI_BOOL ST_DoCaptureJPGProcExt(MI_U16 u16Width, MI_U16 u16Hight, MI_SYS_Rotate_e enRotation)
{
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_RGN_ChnPortParam_t stRgnChnPortParam;
    MI_RGN_HANDLE hRgnHandle;
    MI_RGN_ChnPort_t stRgnChnPort;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_VENC_ChnAttr_t stChnAttr;
    ST_Sys_BindInfo_T stBindInfo;
    MI_SYS_WindowRect_t stRect;
    MI_RGN_Attr_t stRgnAttr;
    MI_RGN_ChnPortParam_t stChnPortParam;
    MI_U32 u32Width = u16Width;
    MI_U32 u32Height = u16Hight;

    if (pstStreamAttr[0].bEnable == TRUE)
    {
        printf("Stream0 is used, IMI mode not support!\n");
        return FALSE;
    }

#if 0
    printf("=======================begin mma_heap_name0=================\n");
    system("cat /proc/mi_modules/mi_sys_mma/mma_heap_name0");
    printf("=======================begin mma_heap_name0=================\n");
#endif

    // port 0 can not scale, set cap width/height
    CanvasScopeLock ScopeLock;

    stRect.u16Width = u16Width;
    stRect.u16Height = u16Hight;
    stRect.u16X = 0;
    stRect.u16Y = 0;
    ExecFunc(MI_VPE_SetPortCrop(0, 0, &stRect), MI_SUCCESS);
    memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
    stVpePortInfo.DepVpeChannel = 0;
    stVpePortInfo.u16OutputWidth = u16Width;
    stVpePortInfo.u16OutputHeight = u16Hight;
    stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
    stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    STCHECKRESULT(ST_Vpe_StartPort(0, &stVpePortInfo));
    memset(&stRgnChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    hRgnHandle = RGN_OSD_HANDLE;
    stRgnChnPortParam.stPoint.u32X = u32Width - RGN_OSD_TIME_WIDTH - 10;
    stRgnChnPortParam.stPoint.u32Y = 10;
    stRgnChnPortParam.bShow = TRUE;
    // fix at vpe port 0
    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stRgnChnPort.eModId = E_MI_RGN_MODID_VPE;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = pstStreamAttr[0].u32InputChn;;
    stRgnChnPort.s32OutputPortId = pstStreamAttr[0].u32InputPort;
    stRgnChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
    stRgnChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
    stRgnChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
    ExecFunc(MI_RGN_AttachToChn(hRgnHandle, &stRgnChnPort, &stRgnChnPortParam), MI_RGN_OK);

    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[0].u32Cover1Handle, &stRgnAttr), MI_RGN_OK);
    stRgnChnPort.eModId = E_MI_RGN_MODID_VPE;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = pstStreamAttr[0].u32InputChn;
    stRgnChnPort.s32OutputPortId = pstStreamAttr[0].u32InputPort;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 100;
    stChnPortParam.stPoint.u32Y = 100;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[0].u32Cover1Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 255, 0);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[0].u32Cover1Handle, &stRgnChnPort, &stChnPortParam), MI_RGN_OK);
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[0].u32Cover2Handle, &stRgnAttr), MI_RGN_OK);
    stRgnChnPort.eModId = E_MI_RGN_MODID_VPE;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = pstStreamAttr[0].u32InputChn;
    stRgnChnPort.s32OutputPortId = pstStreamAttr[0].u32InputPort;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 400;
    stChnPortParam.stPoint.u32Y = 400;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[0].u32Cover2Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 0, 255);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[0].u32Cover2Handle, &stRgnChnPort, &stChnPortParam), MI_RGN_OK);

    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = u32Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = u32Height;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = u32Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32Height;
    stChnAttr.stVeAttr.stAttrJpeg.bByFrame = TRUE;
    stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
    stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateNum = 30;
    stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateDen = 1;
    STCHECKRESULT(ST_Venc_CreateChannel(VENC_CHN_FOR_CAPTURE, &stChnAttr));

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    ExecFunc(MI_VENC_GetChnDevid(VENC_CHN_FOR_CAPTURE, &stBindInfo.stDstChnPort.u32DevId), MI_SUCCESS);
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32ChnId = VENC_CHN_FOR_CAPTURE;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = pstStreamAttr[0].u32FrameRate;
    stBindInfo.u32DstFrmrate = pstStreamAttr[0].u32FrameRate;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    ST_CaptureJPGProc(VENC_CHN_FOR_CAPTURE);

#if 0
    printf("=======================end mma_heap_name0=================\n");
    system("cat /proc/mi_modules/mi_sys_mma/mma_heap_name0");
    printf("=======================end mma_heap_name0=================\n");
#endif

    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    STCHECKRESULT(ST_Venc_DestoryChannel(VENC_CHN_FOR_CAPTURE));
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[0].u32Cover2Handle), MI_RGN_OK);
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[0].u32Cover1Handle), MI_RGN_OK);

    hRgnHandle = RGN_OSD_HANDLE;
    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stRgnChnPort.eModId = E_MI_RGN_MODID_VPE;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = pstStreamAttr[0].u32InputChn;
    stRgnChnPort.s32OutputPortId = pstStreamAttr[0].u32InputPort;
    ExecFunc(MI_RGN_DetachFromChn(hRgnHandle, &stRgnChnPort), MI_RGN_OK);

    STCHECKRESULT(ST_Vpe_StopPort(0, 0));

    return 0;
}

MI_BOOL ST_DoCaptureJPGProc(MI_U16 u16Width, MI_U16 u16Height, MI_SYS_Rotate_e enRotation)
{
    ST_VPE_PortInfo_T stVpePortInfo;
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;
    ST_Sys_BindInfo_T stBindInfo;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_RGN_ChnPort_t stRgnChnPort;
    MI_RGN_HANDLE hRgnHandle;
    ST_Stream_Attr_T *pstStreamAttr = g_stStreamAttr;
    MI_SYS_WindowRect_t stRect;
    MI_RGN_Attr_t stRgnAttr;
    MI_RGN_ChnPortParam_t stChnPortParam;

    MI_U32 u32Width = 0;
    MI_U32 u32Height = 0;

    // port 0 can not scale, set cap width/height
    CanvasScopeLock ScopeLock;

    if (enRotation == E_MI_SYS_ROTATE_90 || enRotation == E_MI_SYS_ROTATE_270)
    {
        u32Width = ALIGN_DOWN(u16Height, 16);
        u32Height = u16Width;

        stRect.u16Width = ALIGN_DOWN(g_u32CapHeight, 16);
        stRect.u16Height = g_u32CapWidth;
        stRect.u16X = 0;
        stRect.u16Y = 0;
        ExecFunc(MI_VPE_SetPortCrop(0, 0, &stRect), MI_SUCCESS);
        memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
        stVpePortInfo.DepVpeChannel = 0;
        stVpePortInfo.u16OutputWidth = ALIGN_DOWN(g_u32CapHeight, 16);
        stVpePortInfo.u16OutputHeight = g_u32CapWidth;
        stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    }
    else
    {
        u32Width = u16Width;
        u32Height = u16Height;

        memset(&stVpePortInfo, 0, sizeof(ST_VPE_PortInfo_T));
        stVpePortInfo.DepVpeChannel = 0;
        stVpePortInfo.u16OutputWidth = g_u32CapWidth;
        stVpePortInfo.u16OutputHeight = g_u32CapHeight;
        stVpePortInfo.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stVpePortInfo.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
    }

    if (pstStreamAttr[0].bEnable == FALSE)
    {
        STCHECKRESULT(ST_Vpe_StartPort(0, &stVpePortInfo));
    }

    memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    stDivpChnAttr.bHorMirror            = FALSE;
    stDivpChnAttr.bVerMirror            = FALSE;
    stDivpChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType           = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X       = 0;
    stDivpChnAttr.stCropRect.u16Y       = 0;
    stDivpChnAttr.stCropRect.u16Width   = 0;
    stDivpChnAttr.stCropRect.u16Height  = 0;
    stDivpChnAttr.u32MaxWidth           = u32Width;
    stDivpChnAttr.u32MaxHeight          = u32Height;

    ExecFunc(MI_DIVP_CreateChn(DIVP_CHN_FOR_OSD, &stDivpChnAttr), MI_SUCCESS);
    ExecFunc(MI_DIVP_StartChn(DIVP_CHN_FOR_OSD), MI_SUCCESS);

    memset(&stOutputPortAttr, 0, sizeof(stOutputPortAttr));
    stOutputPortAttr.eCompMode          = E_MI_SYS_COMPRESS_MODE_NONE;
    stOutputPortAttr.ePixelFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stOutputPortAttr.u32Width           = u32Width;
    stOutputPortAttr.u32Height          = u32Height;

    ST_DBG("u32Width:%d,u32Height:%d\n", u32Width, u32Height);
    STCHECKRESULT(MI_DIVP_SetOutputPortAttr(DIVP_CHN_FOR_OSD, &stOutputPortAttr));
    // bind VPE to divp
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;

    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_FOR_OSD;
    stBindInfo.stDstChnPort.u32PortId = 0;

    // stBindInfo.u32SrcFrmrate = pstStreamAttr[0].u32FrameRate;
    // stBindInfo.u32DstFrmrate = pstStreamAttr[0].u32FrameRate;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    InitRGN();

    hRgnHandle = RGN_OSD_HANDLE2;
    stChnPortParam.stPoint.u32X = 0;
    stChnPortParam.stPoint.u32Y = 0;
    stChnPortParam.bShow = TRUE;
    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));
    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = DIVP_CHN_FOR_OSD;
    stRgnChnPort.s32OutputPortId = 0;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
    stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
    ExecFunc(MI_RGN_AttachToChn(hRgnHandle, &stRgnChnPort, &stChnPortParam), MI_RGN_OK);
#if 0
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[0].u32Cover1Handle, &stRgnAttr), MI_RGN_OK);
    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = DIVP_CHN_FOR_OSD;
    stRgnChnPort.s32OutputPortId = 0;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 100;
    stChnPortParam.stPoint.u32Y = 100;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[0].u32Cover1Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 255, 0);
    //stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.eAlphaMode = E_MI_RGN_PIXEL_ALPHA;
    //stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8BgAlpha = 0;
    //stChnPortParam.unPara.stOsdChnPort.stOsdAlphaAttr.stAlphaPara.stArgb1555Alpha.u8FgAlpha = 0xFF;
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[0].u32Cover1Handle, &stRgnChnPort, &stChnPortParam), MI_RGN_OK);
    memset(&stRgnAttr, 0, sizeof(MI_RGN_Attr_t));
    stRgnAttr.eType = E_MI_RGN_TYPE_COVER;
    ExecFunc(MI_RGN_Create(pstStreamAttr[0].u32Cover2Handle, &stRgnAttr), MI_RGN_OK);
    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = DIVP_CHN_FOR_OSD;
    stRgnChnPort.s32OutputPortId = 0;
    memset(&stChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stChnPortParam.bShow = TRUE;
    stChnPortParam.stPoint.u32X = 400;
    stChnPortParam.stPoint.u32Y = 400;
    stChnPortParam.unPara.stCoverChnPort.u32Layer = pstStreamAttr[0].u32Cover2Handle;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Width = 800;
    stChnPortParam.unPara.stCoverChnPort.stSize.u32Height = 800;
    stChnPortParam.unPara.stCoverChnPort.u32Color = RGB_TO_CRYCB(0, 0, 255);
    ExecFunc(MI_RGN_AttachToChn(pstStreamAttr[0].u32Cover2Handle, &stRgnChnPort, &stChnPortParam), MI_RGN_OK);
#endif
    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    stChnAttr.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGFIXQP;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth = u32Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight = u32Height;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth = u32Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight = u32Height;
    stChnAttr.stVeAttr.stAttrJpeg.bByFrame = TRUE;
    stChnAttr.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
    STCHECKRESULT(ST_Venc_CreateChannel(VENC_CHN_FOR_CAPTURE, &stChnAttr));
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = DIVP_CHN_FOR_OSD;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    ExecFunc(MI_VENC_GetChnDevid(VENC_CHN_FOR_CAPTURE, &stBindInfo.stDstChnPort.u32DevId), MI_SUCCESS);
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VENC;
    stBindInfo.stDstChnPort.u32ChnId = VENC_CHN_FOR_CAPTURE;
    stBindInfo.stDstChnPort.u32PortId = 0;
    // stBindInfo.u32SrcFrmrate = pstStreamAttr[0].u32FrameRate;
    // stBindInfo.u32DstFrmrate = pstStreamAttr[0].u32FrameRate;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
#if 0    
    ST_CaptureJPGProc(VENC_CHN_FOR_CAPTURE);

    // system("echo dumpFrontBuf 1 0 0 /mnt/i6 > /proc/mi_modules/mi_rgn/mi_rgn0");
    // sleep(60);

    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    STCHECKRESULT(ST_Venc_DestoryChannel(VENC_CHN_FOR_CAPTURE));
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[0].u32Cover2Handle), MI_RGN_OK);
    ExecFunc(MI_RGN_Destroy(pstStreamAttr[0].u32Cover1Handle), MI_RGN_OK);
    memset(&stRgnChnPort, 0, sizeof(MI_RGN_ChnPort_t));

    hRgnHandle = RGN_OSD_HANDLE;
    stRgnChnPort.eModId = E_MI_RGN_MODID_DIVP;
    stRgnChnPort.s32DevId = 0;
    stRgnChnPort.s32ChnId = DIVP_CHN_FOR_OSD;
    stRgnChnPort.s32OutputPortId = 0;
    ExecFunc(MI_RGN_DetachFromChn(hRgnHandle, &stRgnChnPort), MI_RGN_OK);

    ExecFunc(MI_DIVP_StopChn(DIVP_CHN_FOR_OSD), MI_SUCCESS);
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DIVP;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = DIVP_CHN_FOR_OSD;
    stBindInfo.stDstChnPort.u32PortId = 0;

    // stBindInfo.u32SrcFrmrate = pstStreamAttr[0].u32FrameRate;
    // stBindInfo.u32DstFrmrate = pstStreamAttr[0].u32FrameRate;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
    
    ExecFunc(MI_DIVP_DestroyChn(DIVP_CHN_FOR_OSD), MI_SUCCESS);
    if (pstStreamAttr[0].bEnable == FALSE)
    {
        STCHECKRESULT(ST_Vpe_StopPort(0, 0));
    }
#endif
    return 0;
}