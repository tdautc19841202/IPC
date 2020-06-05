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

/*
*   mi_isp_datatype.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_DATATYPE_H_
#define _MI_ISP_DATATYPE_H_

#include "mi_common.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define MI_ISP_AUTO_NUM                 (16)
#define MI_ISP_AWB_LV_CT_TBL_NUM        (18)
#define MI_ISP_AWB_CT_TBL_NUM           (10)
#define MI_ISP_AWB_WEIGHT_WIN_NUM       (81)

/************************************* IQ  - Define the structure declaration - START *************************************/
typedef enum __attribute__ ((aligned (4)))
{
    SS_FALSE = 0,
    SS_TRUE = !SS_FALSE,
    SS_BOOL_MAX
} MI_ISP_BOOL_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_OP_TYP_AUTO = 0,
    SS_OP_TYP_MANUAL = !SS_OP_TYP_AUTO,
    SS_OP_TYP_MODE_MAX
} MI_ISP_OP_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_ISP_STATE_NORMAL = 0,
    SS_ISP_STATE_PAUSE = 1,
    SS_ISP_STATE_MAX
} MI_ISP_SM_STATE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_IQ_PARAM_MODE_NORMAL = 0,
    SS_IQ_PARAM_MODE_NIGHT = 1,      //Night Mode
    SS_IQ_PARAM_MODE_MAX
} MI_ISP_IQ_PARAM_MODE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_BYPASS_OFF = 0,
    SS_BYPASS_ON,
    SS_BYPASS_MAX
} MI_ISP_BYPASS_MODE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_IQ_INDEX_0 = 0,
    SS_IQ_INDEX_1,
    SS_IQ_INDEX_2,
    SS_IQ_INDEX_3,
    SS_IQ_INDEX_4,
    SS_IQ_INDEX_5,
    SS_IQ_INDEX_6,
    SS_IQ_INDEX_7,
    SS_IQ_INDEX_8,
    SS_IQ_INDEX_9,
    SS_IQ_INDEX_10,
    SS_IQ_INDEX_11,
    SS_IQ_INDEX_12,
    SS_IQ_INDEX_13,
    SS_IQ_INDEX_14,
    SS_IQ_INDEX_15,
    SS_IQ_INDEX_MAX
} MI_ISP_IQ_INDEX_e;

typedef enum __attribute__ ((aligned (4)))
{
        API20_AUTO = 0,   //[00]
        //API2.0
        API20_FPN,        //[01]
        API20_CCM,        //[02]
        API20_SAT,        //[03]
        API20_OBC,        //[04]
        API20_OBC_P1,     //[05]
        API20_FCOLOR,     //[06]
        API20_CR,         //[07]
        API20_NR_DeSpike, //[08]
        API20_SHP2,       //[09] YEE, ALUT
        API20_NR_3D,      //[10]
        API20_BRI,        //[11]
        API20_LIG,        //[12]
        API20_CST,        //[13]
        API20_GMA,        //[14]
        API20_CTG,        //[15]
        API20_NR_NLM,     //[16]
        API20_DEFOG,      //[17]
        API20_DRC,        //[18]
        API20_DPC,        //[19]
        API20_HSV,        //[20]
        API20_WDR_LOC,    //[21]
        API20_RGBIR,      //[22]
        API20_YUVGMA,     //[23]
        API20_HDR16to12,  //[24]
        API20_COLORTRANS, //[25]
        API20_HDR,        //[26]
        API20_EFFECT,     //[27]
        API20_PFC,        //[28]
        API20_NRChroma,   //[29]
        API20_DM,         //[30]
        API20_NR_3D_P1,   //[31]
        API20_WDRCurveAdv,//[32]
        API20_MAX,        //for para reset
} MI_ISP_API_ID_e;

typedef struct IQ_VERSION_INFO_PARAM_s
{
    MI_U32 u32Vendor;
    MI_U32 u32Major;
    MI_U32 u32Minor;
} IQ_VERSION_INFO_PARAM_t;

typedef struct MI_ISP_IQ_VERSION_INFO_TYPE_s
{
    IQ_VERSION_INFO_PARAM_t stParaAPI;
} MI_ISP_IQ_VERSION_INFO_TYPE_t;

typedef struct IQ_PARAM_INIT_INFO_PARAM_s
{
    MI_ISP_BOOL_e bFlag;
} IQ_PARAM_INIT_INFO_PARAM_t;

typedef struct MI_ISP_IQ_PARAM_INIT_INFO_TYPE_s
{
    IQ_PARAM_INIT_INFO_PARAM_t stParaAPI;
} MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t;

typedef struct FAST_MODE_PARAM_s
{
    MI_ISP_BOOL_e bFastMode;
} FAST_MODE_PARAM_t;

typedef struct MI_ISP_IQ_FAST_MODE_TYPE_s
{
    FAST_MODE_PARAM_t stParaAPI;
} MI_ISP_IQ_FAST_MODE_TYPE_t;

typedef struct MI_ISP_IQ_COLORTOGRAY_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
} MI_ISP_IQ_COLORTOGRAY_TYPE_t;

typedef struct LEVEL_BASE_PARAM_s
{
    MI_U32 u32Lev;                      //0 ~ 100
} LEVEL_BASE_PARAM_t;

typedef struct LIGHTNESS_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} LIGHTNESS_MANUAL_ATTR_t;

typedef struct CONTRAST_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} CONTRAST_MANUAL_ATTR_t;

typedef struct CONTRAST_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} CONTRAST_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_CONTRAST_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    CONTRAST_AUTO_ATTR_t stAuto;
    CONTRAST_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_CONTRAST_TYPE_t;

typedef struct BRIGHTNESS_MANUAL_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI;
} BRIGHTNESS_MANUAL_ATTR_t;

typedef struct BRIGHTNESS_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} BRIGHTNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_BRIGHTNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    BRIGHTNESS_AUTO_ATTR_t stAuto;
    BRIGHTNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_BRIGHTNESS_TYPE_t;

typedef struct LIGHTNESS_AUTO_ATTR_s
{
    LEVEL_BASE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} LIGHTNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_LIGHTNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              // 0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          // M_AUTO ~ (M_MODMAX-1)
    LIGHTNESS_AUTO_ATTR_t stAuto;
    LIGHTNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_LIGHTNESS_TYPE_t;

typedef struct RGBGAMMA_PARAM_s
{
    MI_U16 u16LutR[256];                //0 ~ 4095
    MI_U16 u16LutG[256];                //0 ~ 4095
    MI_U16 u16LutB[256];                //0 ~ 4095
} RGBGAMMA_PARAM_t;

typedef struct RGBGAMMA_MANUAL_ATTR_s
{
    RGBGAMMA_PARAM_t stParaAPI;
} RGBGAMMA_MANUAL_ATTR_t;

typedef struct RGBGAMMA_AUTO_ATTR_s
{
    RGBGAMMA_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBGAMMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBGAMMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBGAMMA_AUTO_ATTR_t stAuto;
    RGBGAMMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBGAMMA_TYPE_t;

typedef struct YUVGAMMA_PAMAR_s
{
    MI_U16 u16LutY[256];                //0 ~ 1023
    MI_U16 u16LutU[128];                //0 ~  511
    MI_U16 u16LutV[128];                //0 ~  511
} YUVGAMMA_PAMAR_t;

typedef struct YUVGAMMA_MANUAL_ATTR_s
{
    YUVGAMMA_PAMAR_t stParaAPI;
} YUVGAMMA_MANUAL_ATTR_t;

typedef struct YUVGAMMA_AUTO_ATTR_s
{
    YUVGAMMA_PAMAR_t stParaAPI[MI_ISP_AUTO_NUM];
} YUVGAMMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_YUVGAMMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    YUVGAMMA_AUTO_ATTR_t stAuto;
    YUVGAMMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_YUVGAMMA_TYPE_t;

typedef struct SATURATION_PARAM_s
{
    MI_U8 u8SatAllStr;                  //0 ~ 127 (32 = 1X)
    MI_U8 u8SatByYSFTAdv[5];            //0 ~ 8
    MI_U8 u8SatByYLUTAdv[6];            //0 ~ 128
    MI_U8 u8SatBySSFTAdv[5];            //0 ~ 8
    MI_U8 u8SatBySLUTAdv[6];            //0 ~ 128
    MI_U8 u8SatCoring;
} SATURATION_PARAM_t;

typedef struct SATURATION_MANUAL_ATTR_s
{
    SATURATION_PARAM_t stParaAPI;
} SATURATION_MANUAL_ATTR_t;

typedef struct SATURATION_AUTO_ATTR_s
{
    SATURATION_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} SATURATION_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_SATURATION_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    SATURATION_AUTO_ATTR_t stAuto;
    SATURATION_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_SATURATION_TYPE_t;

typedef struct MI_ISP_IQ_DEFOG_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //0 ~ 1  manual:fix strength without linking with defog statistics
    MI_U8 u8FlatThrd;                   //0 ~ 255
    MI_U8 u8DensityThrd;                //0 ~ 255
    MI_U8 u8ColorSatByColorSatX[8];     //[0]: 0~255 [1~7]:6~12
    MI_U8 u8ColorSatByColorSatY[8];     //0 ~ 255
    MI_U8 u8ColorSatByDefogGainX[8];    //[0]: 0~255 [1~7]:8~14
    MI_U8 u8ColorSatByDefogGainY[8];    //0 ~ 255
    MI_U8 u8Strength; //0 ~ 100
} MI_ISP_IQ_DEFOG_TYPE_t;

typedef struct RGBMATRIX_PARAM_s
{
    MI_U16 u16CCTthr;
    MI_U16 u16CCM[9];
    MI_U8  u8CCMSat;                    //0 ~ 100 0:Unit matrix, 100:User matrix
} RGBMATRIX_PARAM_t;

typedef struct RGBMATRIX_MANUAL_ATTR_s
{
    MI_U16 u16CCM[9];                   //0 ~ 8191(1024 = 1X)
    MI_U8  u8CCMSat;                    //0 ~ 100 0:Unit matrix, 100:User matrix
} RGBMATRIX_MANUAL_ATTR_t;

typedef struct RGBMATRIX_AUTO_ATTR_s
{
    MI_ISP_BOOL_e bISOActEn;            //0 ~ 1
    RGBMATRIX_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBMATRIX_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBMATRIX_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBMATRIX_AUTO_ATTR_t stAuto;
    RGBMATRIX_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBMATRIX_TYPE_t;

typedef struct FALSECOLOR_PARAM_s
{
    MI_U8 u8FreqThrd;                 //0~255 default:140
    MI_U8 u8EdgeScoreThrd;            //0~255 default:31
    MI_U8 u8ChromaThrdOfStrengthMax;  //0~127 default:10
    MI_U8 u8ChromaThrdOfStrengthMid;  //0~127 default:40
    MI_U8 u8ChromaThrdOfStrengthMin;  //0~127 default:80
    MI_U8 u8StrengthMid;              //0~7   default:7
    MI_U8 u8StrengthMin;              //0~7   default:7
} FALSECOLOR_PARAM_t;

typedef struct FALSECOLOR_MANUAL_ATTR_s
{
    FALSECOLOR_PARAM_t stParaAPI;
} FALSECOLOR_MANUAL_ATTR_t;

typedef struct FALSECOLOR_AUTO_ATTR_s
{
    FALSECOLOR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} FALSECOLOR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_FALSECOLOR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    FALSECOLOR_AUTO_ATTR_t stAuto;
    FALSECOLOR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_FALSECOLOR_TYPE_t;

typedef struct NR3D_PARAM_s
{
    MI_U16 u16MdThd;            //0~2048
    MI_U16 u16MdGain;           //0~2048
    MI_U8  u8TfStr;             //0~64
    MI_U8  u8TfStrEx;           //0~64

    MI_U8  u8MdThdByY[16];      //0~255
    MI_U8  u8MdGainByY[16];     //0~255
    MI_U8  u8M2SLut[16];        //0~31
    MI_U8  u8TfLut[16];         //0~63

    //Y SF (BNR)
    MI_U8 u8YSfStr;             //0~31
    MI_U8 u8YSfBlendLut[16];    //0~16

    //MS_U8  u8CSfStr;             //0 ~ 255
    //MS_U8  u8CSfExStr;           //0 ~ 255
    //MS_U8  u8CSfExBlendGain;     //0 ~ 255
    //MS_U16 u16CSfExBlendClip;    //0 ~ 511

    //MS_U16 u16ShpBlendLut[16];   //0 ~ 1023
} NR3D_PARAM_t;

typedef struct NR3D_MANUAL_ATTR_s
{
    NR3D_PARAM_t stParaAPI;
} NR3D_MANUAL_ATTR_t;

typedef struct NR3D_AUTO_ATTR_s
{
    NR3D_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NR3D_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NR3D_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NR3D_AUTO_ATTR_t stAuto;
    NR3D_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NR3D_TYPE_t;

typedef struct NR3D_P1_PARAM_s
{
    MI_U16 u16MdThd;            //0~2048
    MI_U16 u16MdGain;           //0~2048
    MI_U8  u8TfStr;             //0~64
    MI_U8  u8TfStrEx;           //0~64
    MI_U8  u8MdThdByY[16];      //0~255
    MI_U8  u8MdGainByY[16];     //0~255
    MI_U8  u8M2SLut[16];        //0~31
    MI_U8  u8TfLut[16];         //0~63
} NR3D_P1_PARAM_t;

typedef struct NR3D_P1_MANUAL_ATTR_s
{
    NR3D_P1_PARAM_t stParaAPI;
} NR3D_P1_MANUAL_ATTR_t;

typedef struct NR3D_P1_AUTO_ATTR_s
{
    NR3D_P1_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NR3D_P1_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NR3D_P1_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NR3D_P1_AUTO_ATTR_t stAuto;
    NR3D_P1_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NR3D_P1_TYPE_t;

typedef struct NRDESPIKE_PARAM_s
{
    MI_U8  u8BlendRatio;                //0 ~ 15
    MI_U8  u8StrengthCenterNeighbor;    //0 ~ 5
    MI_U8  u8StrengthMeanStd;           //0 ~ 5
    MI_U8  u8StrengthCornerCross;       //0 ~ 5
    MI_U8  u8DiffGainMeanStd;           //0 ~ 31
    MI_U16 u16DiffGainCenterNeighbor;   //0 ~ 255
    MI_U16 u16DiffThdCornerCross;       //0 ~ 255
} NRDESPIKE_PARAM_t;

typedef struct NRDESPIKE_MANUAL_ATTR_s
{
    NRDESPIKE_PARAM_t stParaAPI;
} NRDESPIKE_MANUAL_ATTR_t;

typedef struct NRDESPIKE_AUTO_ATTR_s
{
    NRDESPIKE_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRDESPIKE_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRDESPIKE_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NRDESPIKE_AUTO_ATTR_t stAuto;
    NRDESPIKE_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRDESPIKE_TYPE_t;

typedef struct NRLUMA_PARAM_s
{
    MI_ISP_BOOL_e bLumaAdjEn;
    MI_U8   u8LumaX[9];
    MI_U16  u16LumaStrByY[10];
    MI_U8   u8Wei[20];
} NRLUMA_PARAM_t;

typedef struct NRLUMA_MANUAL_ATTR_s
{
    NRLUMA_PARAM_t stParaAPI;
} NRLUMA_MANUAL_ATTR_t;

typedef struct NRLUMA_AUTO_ATTR_s
{
    NRLUMA_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRLUMA_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRLUMA_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    NRLUMA_AUTO_ATTR_t stAuto;
    NRLUMA_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRLUMA_TYPE_t;

typedef struct NRChroma_PARAM_s
{
    MI_U8  u8MatchRatio;             //0 ~ 127
    MI_U16 u16UvTh;                  //0 ~ 1023
    //MI_U8  u8CntTh;                  //0 ~ 255
    MI_U16 u16StrengthByCEdge[6];    //0 ~ 1023
} NRChroma_PARAM_t;

typedef struct NRChroma_MANUAL_ATTR_s
{
    NRChroma_PARAM_t stParaAPI;
} NRChroma_MANUAL_ATTR_t;

typedef struct NRChroma_AUTO_ATTR_s
{
    NRChroma_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} NRChroma_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_NRChroma_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;    //M_AUTO ~ (M_MODMAX-1)
    NRChroma_AUTO_ATTR_t stAuto;
    NRChroma_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_NRChroma_TYPE_t;

typedef struct SHARPNESS_LOCAL_PARAM_s
{
    MI_U8 u8OverShootGain;              //0 ~ 255, def: 128
    MI_U8 u8UnderShootGain;             //0 ~ 255, def: 128
    MI_U8 u8CorLUT[6];                  //0 ~ 255, def: 0
    MI_U8 u8SclLUT[6];                  //0 ~ 255, def: 0
} SHARPNESS_LOCAL_PARAM_t;

typedef struct SHARPNESS_PARAM_s
{
    SHARPNESS_LOCAL_PARAM_t stEdgeCtrl;
    SHARPNESS_LOCAL_PARAM_t stTextureCtrl;
    MI_U8 u8EdgeKillLUT[6];             //0 ~ 255,  def: 0
    MI_U8 u8CornerReduce;               //0 ~ 32,   def: 32
    MI_ISP_BOOL_e bDirEn;               //0 ~ 1,    def: 1
    MI_U16 u16SharpnessUD;              //0 ~ 1023
    MI_U16 u16SharpnessD;               //0 ~ 1023
    MI_U16 u16DetailTh;                 //0 ~ 4095
    MI_U8  u8DetailByY[6];              //0 ~ 255
    MI_U16 u16OverShootLimit;           //0 ~ 1023
    MI_U16 u16UnderShootLimit;          //0 ~ 1023
} SHARPNESS_PARAM_t;

typedef struct SHARPNESS_MANUAL_ATTR_s
{
    SHARPNESS_PARAM_t stParaAPI;
} SHARPNESS_MANUAL_ATTR_t;

typedef struct SHARPNESS_AUTO_ATTR_s
{
    SHARPNESS_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} SHARPNESS_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_SHARPNESS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    SHARPNESS_AUTO_ATTR_t stAuto;
    SHARPNESS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_SHARPNESS_TYPE_t;

typedef struct CROSSTALK_PARAM_s
{
    //MI_U8  u8StrengthLow;               //0 ~ 7
    //MI_U16 u16ThresholdLow;             //0 ~ 255
    //MI_U8  u8StrengthHigh;              //0 ~ 7
    //MI_U16 u16ThresholdHigh;            //0 ~ 255
    MI_U8  u8StrengthV2;                //0 ~ 31
    MI_U16 u16ThresholdV2;              //0 ~ 255
    MI_U16 u16ThresholdOffsetV2;        //0 ~ 4095
} CROSSTALK_PARAM_t;

typedef struct CROSSTALK_MANUAL_ATTR_s
{
    CROSSTALK_PARAM_t stParaAPI;
} CROSSTALK_MANUAL_ATTR_t;

typedef struct CROSSTALK_AUTO_ATTR_s
{
    CROSSTALK_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} CROSSTALK_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_CROSSTALK_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    //MI_ISP_BOOL_e enV2;                 //0 ~ 1
    CROSSTALK_AUTO_ATTR_t stAuto;
    CROSSTALK_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_CROSSTALK_TYPE_t;

typedef struct OBC_PARAM_s
{
    MI_U16 u16ValR;                     //0 ~ 255
    MI_U16 u16ValGr;                    //0 ~ 255
    MI_U16 u16ValGb;                    //0 ~ 255
    MI_U16 u16ValB;                     //0 ~ 255
} OBC_PARAM_t;

typedef struct OBC_MANUAL_ATTR_s
{
    OBC_PARAM_t stParaAPI;
} OBC_MANUAL_ATTR_t;

typedef struct OBC_AUTO_ATTR_s
{
    OBC_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} OBC_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_OBC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    OBC_AUTO_ATTR_t stAuto;
    OBC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_OBC_TYPE_t;

typedef struct WDR_PARAM_s
{
    MI_U8         u8BoxNum;                 //1 ~ 4
    MI_ISP_BOOL_e bAutoDetailEnhance;       //0 ~ 1
    MI_U8         u8ManualDetailEnhance;    //0~255; default:128
    MI_U8         u8GlobalDarkToneEnhance;  //0 ~ 15
    MI_U8         u8WDRStrByY[33];          //0 ~ 255
    MI_U8         u8Strength;               //0 ~ 255
    MI_U8         u8DarkLimit;              //0 ~ 255
    MI_U8         u8BrightLimit;            //0 ~ 255
    //MI_U8         u8ColorSatByY[33];        //0 ~ 255; center:128
} WDR_PARAM_t;

typedef struct WDR_MANUAL_ATTR_s
{
    WDR_PARAM_t stParaAPI;
} WDR_MANUAL_ATTR_t;

typedef struct WDR_AUTO_ATTR_s
{
    WDR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} WDR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_WDR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    WDR_AUTO_ATTR_t stAuto;
    WDR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_WDR_TYPE_t;

typedef struct WDRCurveAdv_PARAM_s
{
    MI_U16      u16Slope;                //1024~16384
    MI_U8       u8TransitionPoint0;     //0~255
    MI_U8       u8TransitionPoint1;     //0~255
    MI_U8       u8SaturatedPoint;       //0~255
    MI_U8       u8CurveModeSel;          //0~5
} WDRCurveAdv_PARAM_t;

typedef struct WDRCurveAdv_MANUAL_ATTR_s
{
    WDRCurveAdv_PARAM_t stParaAPI;
} WDRCurveAdv_MANUAL_ATTR_t;

typedef struct WDRCurveAdv_AUTO_ATTR_s
{
    WDRCurveAdv_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} WDRCurveAdv_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_WDRCurveAdv_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    WDRCurveAdv_AUTO_ATTR_t stAuto;
    WDRCurveAdv_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_WDRCurveAdv_TYPE_t;


typedef struct DYNAMIC_DP_PARAM_s
{
    MI_ISP_BOOL_e bHotPixEn;            //0 ~ 1
    MI_U16 u16HotPixCompSlpoe;
    MI_ISP_BOOL_e bDarkPixEn;           //0 ~ 1
    MI_U16 u16DarkPixCompSlpoe;
    MI_U16 u16DPCTH;
    MI_ISP_BOOL_e bBlendEn;
    MI_U16 u16DiffLut[6];
    MI_U16 u16YLut[6];
} DYNAMIC_DP_PARAM_t;

typedef struct DYNAMIC_DP_MANUAL_ATTR_s
{
    DYNAMIC_DP_PARAM_t stParaAPI;
} DYNAMIC_DP_MANUAL_ATTR_t;

typedef struct DYNAMIC_DP_AUTO_ATTR_s
{
    DYNAMIC_DP_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} DYNAMIC_DP_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_DYNAMIC_DP_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    DYNAMIC_DP_AUTO_ATTR_t stAuto;
    DYNAMIC_DP_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DYNAMIC_DP_TYPE_t;

typedef struct HSV_PARAM_s
{
    MI_S16 s16HueLut[24];               //-64 ~  64
    MI_U16 u16SatLut[24];               //  0 ~ 255 (64->1X)
    MI_S16 s16HueLut_ForUnitCCM[24];    //-64 ~  64
    MI_U16 u16SatLut_ForUnitCCM[24];    //  0 ~ 255 (64->1X)
    MI_U8  u8GlobalSat;                 //  0 ~ 255 (64->1x)
} HSV_PARAM_t;

typedef struct HSV_MANUAL_ATTR_s
{
    HSV_PARAM_t stParaAPI;
} HSV_MANUAL_ATTR_t;

typedef struct HSV_AUTO_ATTR_s
{
    HSV_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} HSV_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_HSV_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    HSV_AUTO_ATTR_t stAuto;
    HSV_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_HSV_TYPE_t;

typedef struct RGBIR_PARAM_s
{
    MI_U8 u8IrPosType;                  //0 ~ 7, def : 0
    MI_ISP_BOOL_e bRemovelEn;           //0 ~ 1, def : 1
    MI_U16 u16Ratio_R[6];
    MI_U16 u16Ratio_G[6];
    MI_U16 u16Ratio_B[6];
} RGBIR_PARAM_t;

typedef struct RGBIR_MANUAL_ATTR_s
{
    RGBIR_PARAM_t stParaAPI;
} RGBIR_MANUAL_ATTR_t;

typedef struct RGBIR_AUTO_ATTR_s
{
    RGBIR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} RGBIR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_RGBIR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    RGBIR_AUTO_ATTR_t stAuto;
    RGBIR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_RGBIR_TYPE_t;

typedef struct FPN_PARAM_s
{
    MI_U16 u16Start_X;                  //0 ~ 4095
    MI_U16 u16Start_Y;                  //0 ~ 4095
    MI_U16 u16Width;                    //0 ~ 4095
    MI_U8  u8Height;                    //1 ~ 31
    MI_U16 u16Pre_Offset;               //0 ~ 32767
    MI_U8  u8Pre_Offset_Sign;           //0 ~ 1
    MI_U16 u16Diff_TH;                  //0 ~ 65535
    MI_U16 u16Cmp_Ratio;                //0 ~ 4095
    MI_U8  u8Frame_Num;                 //1 ~ 31
    MI_U8  u8SW_Offset_En;              //0 ~ 1
    MI_U16 u16SW_Cmp_Ratio;             //0 ~ 4095
} FPN_PARAM_t;

typedef struct FPN_MANUAL_ATTR_s
{
    FPN_PARAM_t stParaAPI;
} FPN_MANUAL_ATTR_t;

typedef struct FPN_AUTO_ATTR_s
{
    FPN_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} FPN_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_FPN_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //M_AUTO ~ (M_MODMAX-1)
    FPN_AUTO_ATTR_t stAuto;
    FPN_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_FPN_TYPE_t;

typedef struct PFC_PARAM_s
{
    MI_U8  u8Strength;         //0 ~ 128
    MI_U16 u16HueAngleStart;   //0 ~ 359
    MI_U16 u16HueAngleEnd;     //0 ~ 359
    MI_U16 u16SatThreshold;    //0 ~ 1023
    MI_U8  u8YSensitivity;     //0 ~ 100
    MI_U16 u16StrengthByDY[8]; //0 ~ 512
} PFC_PARAM_t;

typedef struct PFC_MANUAL_ATTR_s
{
    PFC_PARAM_t paraAPI;
} PFC_MANUAL_ATTR_t;

typedef struct PFC_AUTO_ATTR_s
{
    PFC_PARAM_t paraAPI[MI_ISP_AUTO_NUM];
} PFC_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_PFC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;    //M_AUTO ~ (M_MODMAX-1)
    PFC_AUTO_ATTR_t stAuto;
    PFC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_PFC_TYPE_t;

typedef struct DEMOSAIC_PARAM_s
{
    MI_U8 u8DirThrd;            //0~63    def:35
    //MI_U16 u16ColorGrayRatio;
    MI_U8 u8EdgeSmoothThrdY;    //0~255   def:230
    MI_U8 u8EdgeSmoothThrdC;    //0~127   def:20
} DEMOSAIC_PARAM_t;

typedef struct DEMOSAIC_MANUAL_ATTR_s
{
    DEMOSAIC_PARAM_t paraAPI;
} DEMOSAIC_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_DEMOSAIC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;            //0 ~ 1
    DEMOSAIC_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_DEMOSAIC_TYPE_t;


typedef struct COLORTRANS_PARAM_s
{
    MI_U16 u16Y_OFST;                   //0 ~ 2047
    MI_U16 u16U_OFST;                   //0 ~ 2047
    MI_U16 u16V_OFST;                   //0 ~ 2047
    MI_U16 u16Matrix[9];                //0 ~ 1023
} COLORTRANS_PARAM_t;

typedef struct COLORTRANS_MANUAL_ATTR_s
{
    COLORTRANS_PARAM_t stParaAPI;
} COLORTRANS_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_COLORTRANS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    COLORTRANS_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_COLORTRANS_TYPE_t;

typedef struct HDR_PARAM_s
{
    MI_U16 u16SensorExpRatio;
    MI_ISP_BOOL_e bNrEn;
    MI_ISP_BOOL_e bDynRatioEn;
    MI_U8  u8CRefSel;
    MI_U16 u16YwtTh1;
    MI_U16 u16YwtTh2;
    //MI_U8  bMotEn;
    //MI_U8  u8MotRefSel;
    //MI_U8  u8MotWtLut[16];
} HDR_PARAM_t;

typedef struct HDR_MANUAL_ATTR_s
{
    HDR_PARAM_t stParaAPI;
} HDR_MANUAL_ATTR_t;

typedef struct HDR_AUTO_ATTR_s
{
    HDR_PARAM_t stParaAPI[MI_ISP_AUTO_NUM];
} HDR_AUTO_ATTR_t;

typedef struct MI_ISP_IQ_HDR_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_OP_TYPE_e enOpType;          //OP_TYP_AUTO ~ (OP_TPY_MODMAX-1)
    HDR_AUTO_ATTR_t stAuto;
    HDR_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_HDR_TYPE_t;

typedef struct EFFECT_PARAM_s
{
    MI_ISP_BOOL_e bEn;
    MI_U8 u8Level;
} EFFECT_PARAM_t;

typedef struct EFFECT_MANUAL_ATTR_s
{
    EFFECT_PARAM_t stParaColorToGrey;   //0 ~ 100
    EFFECT_PARAM_t stParaSepia;         //0 ~ 100
    EFFECT_PARAM_t stParaNegative;      //0 ~ 100
    EFFECT_PARAM_t stParaSatOffset;     //0 ~ 100
    EFFECT_PARAM_t stParaSatVivid;      //0 ~ 100
    EFFECT_PARAM_t stParaSatPale;       //0 ~ 100
    EFFECT_PARAM_t stParaSharpOffset;   //0 ~ 100
} EFFECT_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_EFFECT_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_ISP_BOOL_e bResetFlag;           //0 ~ 1
    EFFECT_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_EFFECT_TYPE_t;

typedef struct MI_ISP_IQ_SYS_MCNR_MEMORY_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
} MI_ISP_IQ_SYS_MCNR_MEMORY_TYPE_t;

typedef struct MI_ISP_IQ_LSC_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    MI_U16 u16TableR[255];
    MI_U16 u16TableG[255];
    MI_U16 u16TableB[255];
} MI_ISP_IQ_LSC_TYPE_t;

typedef struct R2Y_PARAM_s
{
    MI_U16 u16Matrix[9];
    MI_U8  u8AddY16;
} R2Y_PARAM_t;

typedef struct R2Y_MANUAL_ATTR_s
{
    R2Y_PARAM_t stParaAPI;
} R2Y_MANUAL_ATTR_t;

typedef struct MI_ISP_IQ_R2Y_TYPE_s
{
    MI_ISP_BOOL_e bEnable;              //0 ~ 1
    R2Y_MANUAL_ATTR_t stManual;
} MI_ISP_IQ_R2Y_TYPE_t;

typedef struct MI_ISP_API_BYPASS_TYPE_s
{
    MI_ISP_BYPASS_MODE_e bEnable;       //0 ~ 1
    MI_ISP_API_ID_e eAPIIndex;
} MI_ISP_API_BYPASS_TYPE_t;

typedef struct MI_ISP_API_CHANNEL_ID_TYPE_s
{
    MI_U32 u32ChannelID;
} MI_ISP_API_CHANNEL_ID_TYPE_t;

/************************************* IQ  - Define the structure declaration - END   *************************************/

/************************************* AE  - Define the structure declaration - START *************************************/
typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_MODE_A,   // auto
    SS_AE_MODE_AV,  // aperture priority
    SS_AE_MODE_SV,
    SS_AE_MODE_TV,  // shutter priority
    SS_AE_MODE_M,   // manual mode
    SS_AE_MODE_MAX
} MI_ISP_AE_MODE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_WEIGHT_AVERAGE = 0,
    SS_AE_WEIGHT_CENTER,
    SS_AE_WEIGHT_SPOT,
    SS_AE_WEIGHT_MAX
} MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_FLICKER_TYPE_DISABLE = 0,
    SS_AE_FLICKER_TYPE_60HZ = 1,
    SS_AE_FLICKER_TYPE_50HZ = 2,
    SS_AE_FLICKER_TYPE_MAX
} MI_ISP_AE_FLICKER_TYPE_e;

typedef enum __attribute__ ((aligned (4)))
{
    SS_AE_STRATEGY_BRIGHTTONE,
    SS_AE_STRATEGY_DARKTONE,
    SS_AE_STRATEGY_AUTO,
    SS_AE_STRATEGY_MAX
} MI_ISP_AE_STRATEGY_TYPE_e;

typedef struct MI_ISP_AE_HIST_WEIGHT_Y_TYPE_s
{
    MI_U32 u32LumY;
    MI_U32 u32AvgY;
    MI_U32 u32Hits[128];
} MI_ISP_AE_HIST_WEIGHT_Y_TYPE_t;

typedef struct MI_ISP_AE_EXPO_VALUE_TYPE_s
{
    MI_U32 u32FNx10;
    MI_U32 u32SensorGain;
    MI_U32 u32ISPGain;
    MI_U32 u32US;
} MI_ISP_AE_EXPO_VALUE_TYPE_t;

typedef struct MI_ISP_AE_EXPO_INFO_TYPE_s
{
    MI_ISP_BOOL_e bIsStable;
    MI_ISP_BOOL_e bIsReachBoundary;
    MI_ISP_AE_EXPO_VALUE_TYPE_t stExpoValueLong;
    MI_ISP_AE_EXPO_VALUE_TYPE_t stExpoValueShort;
    MI_ISP_AE_HIST_WEIGHT_Y_TYPE_t stHistWeightY;
    MI_U32 u32LVx10;
    MI_S32 s32BV;
    MI_U32 u32SceneTarget;
} MI_ISP_AE_EXPO_INFO_TYPE_t;

typedef struct MI_ISP_AE_EV_COMP_TYPE_s
{
    MI_S32 s32EV;
    MI_U32 u32Grad;
} MI_ISP_AE_EV_COMP_TYPE_t;

typedef struct MI_ISP_AE_INTP_LUT_TYPE_s
{
    MI_U16 u16NumOfPoints;
    MI_U32 u32Y[16];
    MI_U32 u32X[16];
} MI_ISP_AE_INTP_LUT_TYPE_t;

typedef struct CONV_SPEED_PARAM_s
{
    MI_U32 u32SpeedX[4];
    MI_U32 u32SpeedY[4];
} CONV_SPEED_PARAM_t;

typedef struct CONV_THD_PARAM_s
{
    MI_U32 u32InThd;
    MI_U32 u32OutThd;
} CONV_THD_PARAM_t;

typedef struct MI_ISP_AE_CONV_CONDITON_TYPE_s
{
    CONV_THD_PARAM_t stConvThrd;
    CONV_SPEED_PARAM_t stConvSpeed;
} MI_ISP_AE_CONV_CONDITON_TYPE_t;

typedef struct MI_ISP_AE_EXPO_LIMIT_TYPE_s
{
    MI_U32 u32MinShutterUS;
    MI_U32 u32MaxShutterUS;
    MI_U32 u32MinFNx10;
    MI_U32 u32MaxFNx10;
    MI_U32 u32MinSensorGain;
    MI_U32 u32MinISPGain;
    MI_U32 u32MaxSensorGain;
    MI_U32 u32MaxISPGain;
} MI_ISP_AE_EXPO_LIMIT_TYPE_t;

typedef struct EXPO_POINT_PARAM_s
{
    MI_U32 u32FNumx10;
    MI_U32 u32Shutter;
    MI_U32 u32TotalGain;
    MI_U32 u32SensorGain;
} EXPO_POINT_PARAM_t;

typedef struct MI_ISP_AE_EXPO_TABLE_TYPE_s
{
    MI_U32 u32NumPoints;
    EXPO_POINT_PARAM_t stExpoTbl[16];   // LV from High to Low
} MI_ISP_AE_EXPO_TABLE_TYPE_t;

typedef struct WIN_WEIGHT_PARAM_s
{
    MI_U8 u8AverageTbl[32*32];
    MI_U8 u8CenterTbl[32*32];
    MI_U8 u8SpotTbl[32*32];
} WIN_WEIGHT_PARAM_t;

typedef struct MI_ISP_AE_WIN_WEIGHT_TYPE_s
{
    MI_ISP_AE_WIN_WEIGHT_MODE_TYPE_e eTypeID;
    WIN_WEIGHT_PARAM_t stParaAPI;
} MI_ISP_AE_WIN_WEIGHT_TYPE_t;

typedef struct MI_ISP_AE_STRATEGY_TYPE_s
{
    MI_ISP_AE_STRATEGY_TYPE_e   eAEStrategyMode;
    MI_U32                      u32Weighting;
    MI_ISP_AE_INTP_LUT_TYPE_t   stUpperOffset;
    MI_ISP_AE_INTP_LUT_TYPE_t   stLowerOffset;
    MI_U32                      u32BrightToneStrength;
    MI_U32                      u32BrightToneSensitivity;
    MI_U32                      u32DarkToneStrength;
    MI_U32                      u32DarkToneSensitivity;
    MI_U32                      u32AutoStrength;
    MI_U32                      u32AutoSensitivity;
} MI_ISP_AE_STRATEGY_TYPE_t;

typedef struct MI_ISP_AE_RGBIR_AE_TYPE_s
{
    MI_ISP_BOOL_e  bEnable;
    MI_U16       u16MaxYWithIR;
    MI_U16       u16MinISPGainCompRatio;
} MI_ISP_AE_RGBIR_AE_TYPE_t;

typedef struct MI_ISP_AE_HDR_TYPE_s
{
    MI_ISP_AE_INTP_LUT_TYPE_t stAeHdrRatio;
} MI_ISP_AE_HDR_TYPE_t;



/************************** Cus3A ***********************/
typedef struct
{
    MI_U8 uAvgR;
    MI_U8 uAvgG;
    MI_U8 uAvgB;
    MI_U8 uAvgY;
} MI_ISP_AE_AVGS;

typedef struct
{
    MI_U32 nBlkX;
    MI_U32 nBlkY;
    MI_ISP_AE_AVGS nAvg[128*90];
}MI_ISP_AE_HW_STATISTICS_t;

typedef struct
{
    MI_U8 uAvgR;
    MI_U8 uAvgG;
    MI_U8 uAvgB;
} MI_ISP_AWB_AVGS;

typedef struct
{
    MI_U32 nBlkX;
    MI_U32 nBlkY;
    MI_ISP_AWB_AVGS nAvg[128*90];
} MI_ISP_AWB_HW_STATISTICS_t;

typedef struct
{
    MI_U16 nHisto[128];
} MI_ISP_HISTO_HW_STATISTICS_t;
/***************************** end of Cus3A ****************************/

/************************************* AE  - Define the structure declaration - END   *************************************/

/************************************* AWB - Define the structure declaration - START *************************************/
typedef enum __attribute__ ((aligned (4)))
{
    SS_AWB_ALG_DEFAULT = 0,
    SS_AWB_ALG_ADVANCE = 1,
    SS_AWB_ALG_MAX
} MI_ISP_AWB_ALG_TYPE_e;

typedef struct MI_ISP_AWB_QUERY_INFO_TYPE_s
{
    MI_U16 u16Rgain;
    MI_U16 u16Grgain;
    MI_U16 u16Gbgain;
    MI_U16 u16Bgain;
    MI_U16 u16ColorTemp;
    MI_U8  u8WPInd;
    MI_ISP_BOOL_e bMultiLSDetected;
    MI_U8  u8FirstLSInd;
    MI_U8  u8SecondLSInd;
} MI_ISP_AWB_QUERY_INFO_TYPE_t;

typedef struct CT_LIMIT_PARAM_s
{
    MI_U16 u16MaxRgain;                                         //RW, Maximum  RGain, Range: [0, 8191]
    MI_U16 u16MinRgain;                                         //RW, Miniimum RGain, Range: [0, 8191]
    MI_U16 u16MaxBgain;                                         //RW, Maximum  BGain, Range: [0, 8191]
    MI_U16 u16MinBgain;                                         //RW, Miniimum BGain, Range: [0, 8191]
} CT_LIMIT_PARAM_t;

typedef struct CT_WEIGHT_PARAM_s
{
    MI_U16 u16Weight[MI_ISP_AWB_CT_TBL_NUM];                    //RW, Light CT Weight, Range: [1, 255]
} CT_WEIGHT_PARAM_t;

typedef struct CT_RATIO_PARAM_s
{
    MI_U16 u16Ratio[MI_ISP_AWB_CT_TBL_NUM];                     //RW, CT Prefer Ratio, Range: [1, 255]
} CT_RATIO_PARAM_t;

typedef struct AWB_ATTR_PARAM_s
{
    MI_U8  u8Speed;                                             //RW, AWB converging speed, Range: [0x1, 0x64]
    MI_U8  u8Tolerance;                                         //RW, AWB converging threshold, Range:[0, 255], Recommended: [64]
    MI_U16 u16RefColorTemp;                                     //RW, AWB calibration parameter, Color Temperature, Range:[1000, 10000]
    MI_U16 u16RefRgain;                                         //RW, AWB calibration parameter, RGain Value, Range:[256, 4095]
    MI_U16 u16RefBgain;                                         //RW, AWB calibration parameter, BGain Value, Range:[256, 4095]
    MI_U16 u16ZoneSel;                                          //RW, AWB Zone Selection, 0:Global, 1:Color Temperature Area
    MI_ISP_AWB_ALG_TYPE_e eAlgType;                             //RW, AWB algorithm type
    MI_U8  u8RGStrength;                                        //RW, AWB adjust RG ratio, Range:[0, 255]
    MI_U8  u8BGStrength;                                        //RW, AWB adjust BG ratio, Range:[0, 255]
    CT_LIMIT_PARAM_t stCTLimit;                                 //RW, AWB limitation when envirnoment ct is out of boundary
    CT_WEIGHT_PARAM_t stLvWeight[MI_ISP_AWB_LV_CT_TBL_NUM];     //RW, AWB Lv Ct Weight, Range: [0, 255]
    CT_RATIO_PARAM_t stPreferRRatio[MI_ISP_AWB_LV_CT_TBL_NUM];  //RW, AWB prefer R gain, Range: [0, 255]
    CT_RATIO_PARAM_t stPreferBRatio[MI_ISP_AWB_LV_CT_TBL_NUM];  //RW, AWB prefer B gain, Range: [0, 255]
    MI_ISP_BOOL_e bWpWeightEnable;
    MI_U16 u16WpWeight[MI_ISP_AWB_CT_TBL_NUM];
    MI_U32 u32WeightWin[MI_ISP_AWB_WEIGHT_WIN_NUM];             //RW, AWB region gain, Range: [0, 16]
} AWB_ATTR_PARAM_t;

typedef struct MWB_ATTR_PARAM_s
{
    MI_U16 u16Rgain;                                            //RW, Multiplier for R  color channel, Range: [0, 0x2000]
    MI_U16 u16Grgain;                                           //RW, Multiplier for Gr color channel, Range: [0, 0x2000]
    MI_U16 u16Gbgain;                                           //RW, Multiplier for Gb color channel, Range: [0, 0x2000]
    MI_U16 u16Bgain;                                            //RW, Multiplier for B  color channel, Range: [0, 0x2000]
} MWB_ATTR_PARAM_t;

typedef struct MI_ISP_AWB_ATTR_TYPE_s
{
    MI_ISP_SM_STATE_TYPE_e eState;
    MI_ISP_OP_TYPE_e eOpType;
    MWB_ATTR_PARAM_t stManualParaAPI;
    AWB_ATTR_PARAM_t stAutoParaAPI;
} MI_ISP_AWB_ATTR_TYPE_t;

typedef struct AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_s
{
    MI_U16 u16WhiteRgain;                                       //RW, RGain of white Point Location , Range: [256, 4095]
    MI_U16 u16WhiteBgain;                                       //RW, RGain of white Point Location , Range: [256, 4095]
    MI_U8  u8AreaSize;                                          //RW, Light Area Size , Range: [1, 32]
    MI_ISP_BOOL_e bExclude;                                     //RW, Include or exclude Uaer light Area, 0: include, 1:exclude
} AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_t;

typedef struct MI_ISP_AWB_ATTR_EX_TYPE_s
{
    MI_U8 u8AreaScale;                                          //RW, Expand light area , Range: [0, 16], Default: 0
    MI_ISP_BOOL_e bExtraLightEn;
    AWB_ATTR_EXTRA_LIGHTSOURCE_PARAM_t stLightInfo[4];
} MI_ISP_AWB_ATTR_EX_TYPE_t;

typedef struct MI_ISP_AWB_MULTILS_LS_TYPE_s
{
    MI_ISP_BOOL_e bEnable;
    MI_U8  u8Sensitive;
    MI_U8  u8CaliStrength;
    MI_U16 u16CcmForLow[9];
    MI_U16 u16CcmForHigh[9];
} MI_ISP_AWB_MULTILS_LS_TYPE_t;

typedef struct MI_ISP_AWB_CT_WEIGHT_TYPE_s
{
    MI_U16 u16LvIndex;
    CT_WEIGHT_PARAM_t stParaAPI;
} MI_ISP_AWB_CT_WEIGHT_TYPE_t;

typedef struct MI_ISP_AWB_CT_CALI_TYPE_s
{
    MI_U16 u16StartIdx;                                         //RW, Light area start index, Range: [0, u2EndIdx]
    MI_U16 u16EndIdx;                                           //RW, Light area end index, Range: [u2StartIdx, 9]
    MI_U16 u16CtParams[40];                                     //RW, Color temperature of calibration paramters , Range: [1, 1000]
} MI_ISP_AWB_CT_CALI_TYPE_t;

typedef struct MI_ISP_AWB_CT_STATISTICS_TYPE_s
{
    MI_U16 u16Width;                                            //RW, Effective range
    MI_U16 u16Height;                                           //RW, Effective range
    MI_U16 u16StatisX[5760];                                    //RW, Color Temperature Curve Domain Statistics X, max is 64x90
    MI_U16 u16StatisY[5760];                                    //RW, Color Temperature Curve Domain Statistics Y, max is 64x90
} MI_ISP_AWB_CT_STATISTICS_TYPE_t;

typedef struct MI_ISP_AWB_HW_STATISTICS_TYPE_s
{
    MI_U8 u8AwbBuffer[34560]; // 128 * 90 * 3
} MI_ISP_AWB_HW_STATISTICS_TYPE_t;

/************************************* AWB - Define the structure declaration - END   *************************************/

/************************************* AF  - Define the structure declaration - START *************************************/

/************************************* AF  - Define the structure declaration - END   *************************************/
typedef enum __attribute__ ((aligned (1)))
{
    SS_CALI_ITEM_AWB = 0,
    SS_CALI_ITEM_OBC,
    SS_CALI_ITEM_SDC,
    SS_CALI_ITEM_ALSC,
    SS_CALI_ITEM_MAX,
} MI_ISP_CALI_ITEM_e;

typedef struct MI_ISP_EXCUTE_CMD_ARRAY_s
{
    MI_U32 u32CmdType;
    MI_U16 u16APIID;
    MI_S32 (*callback)(MI_U32 Channel, MI_U8 *param_ary[], MI_U8 param_num);
} MI_ISP_EXCUTE_CMD_ARRAY_t;

typedef enum __attribute__ ((aligned (1)))
{
    MI_ISP_CAMERA_CMD_SET = 0,
    MI_ISP_CAMERA_CMD_GET,
    MI_ISP_CAMERA_CMD_SET_MODE ,
    MI_ISP_CAMERA_CMD_GET_MODE,
    MI_ISP_CAMERA_CMD_GET_PIC,
    MI_ISP_CAMERA_CMD_SET_API,
    MI_ISP_CAMERA_CMD_GET_API,
    MI_ISP_CAMERA_CMD_UPLOAD_FILE,    /* client upload file to server */
    MI_ISP_CAMERA_CMD_DOWNLOAD_FILE,  /* client download file from server*/
} MI_ISP_CAMERA_EXT_CMD_TYPE_e;

typedef struct MI_ISP_CMD_HEADER_s
{
    MI_ISP_CAMERA_EXT_CMD_TYPE_e CmdType;
    MI_S32 CmdLen;
} MI_ISP_CMD_HEADER_t;

typedef struct MI_ISP_API_PARAM_s
{
    MI_U32 u32MagicKey;
    MI_ISP_CMD_HEADER_t sCmdheader;
    MI_U16 u16APIID;
    MI_U16 u16ParamNum;
} MI_ISP_API_PARAM_t;

typedef struct MI_ISP_API_BIN_FILE_s
{
    MI_U32 u32FileID;
    MI_U32 u32ISPVer;
    MI_U32 u32DataLen;
    MI_U32 u32Checksum;
    MI_U32 u32MagicKey;
    MI_U32 u32Reserved[3];
} MI_ISP_API_BIN_FILE_t;

/************************** Cus3A ***********************/
typedef struct
{
    MI_BOOL bAE;
    MI_BOOL bAWB;
    MI_BOOL bAF;
}Cus3AEnable_t;

typedef struct
{
    MI_U32 Size;                   /**< struct size*/
    char sensor_id[32];            /**< sensor module id*/
    MI_U32 shutter;                /**< shutter Shutter in ns*/
    MI_U32 shutter_step;           /**< shutter Shutter step ns*/
    MI_U32 shutter_min;            /**< shutter Shutter min us*/
    MI_U32 shutter_max;            /**< shutter Shutter max us*/
    MI_U32 sensor_gain;            /**< sensor_gain Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gain_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
    MI_U32 isp_gain;               /**< isp_gain Isp digital gain , 1X = 1024 */
    MI_U32 isp_gain_max;           /**< isp_gain Maximum Isp digital gain , 1X = 1024 */
    MI_U32 FNx10;                  /**< F number * 10*/
    MI_U32 fps;                    /**< initial frame per second*/
    MI_U32 shutterHDRShort_step;           /**< shutter Shutter step ns*/
    MI_U32 shutterHDRShort_min;            /**< shutter Shutter min us*/
    MI_U32 shutterHDRShort_max;            /**< shutter Shutter max us*/
    MI_U32 sensor_gainHDRShort_min;        /**< sensor_gain_min Minimum Sensor gain, 1X = 1024*/
    MI_U32 sensor_gainHDRShort_max;        /**< sensor_gain_max Maximum Sensor gain, 1X = 1024*/
}CusAEInitParam_t,CusAeInitParam_t;

/*! @brief ISP report to AE, hardware statistic */
typedef struct
{
    MI_U32 Size;       /**< struct size*/
    void   *hist1;   /**< HW statistic histogram 1*/
    void   *hist2;   /**< HW statistic histogram 2*/
    MI_U32 AvgBlkX;  /**< HW statistics average block number*/
    MI_U32 AvgBlkY;  /**< HW statistics average block number*/
    void * avgs; /**< HW statistics average block data*/
    MI_U32 Shutter;                    /**< Current shutter in ns*/
    MI_U32 SensorGain;                 /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGain;                    /**< Current ISP gain, 1X = 1024*/
    MI_U32 ShutterHDRShort;           /**< Current shutter in ns*/
    MI_U32 SensorGainHDRShort;        /**< Current Sensor gain, 1X = 1024 */
    MI_U32 IspGainHDRShort;           /**< Current ISP gain, 1X = 1024*/

} __attribute__((packed, aligned(1))) CusAEInfo_t, CusAeInput_t;

/*! @brief ISP ae algorithm result*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 Change;         /**< if true, apply this result to hw register*/
    MI_U32 Shutter;         /**< Shutter in ns */
    MI_U32 SensorGain;      /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGain;         /**< ISP gain, 1X = 1024 */
    MI_U32 ShutterHdrShort;     /**< Shutter in ns */
    MI_U32 SensorGainHdrShort;  /**< Sensor gain, 1X = 1024 */
    MI_U32 IspGainHdrShort;     /**< ISP gain, 1X = 1024 */
    MI_U32 u4BVx16384;      /**< Bv * 16384 in APEX system, EV = Av + Tv = Sv + Bv */
    MI_U32 AvgY;            /**< frame brightness */
    MI_U32 HdrRatio;   /**< hdr ratio, 1X = 1024 */
}__attribute__((packed, aligned(1))) CusAEResult_t, CusAeOutput_t;

/*! @brief AWB HW statistics data*/
typedef struct
{
    MI_U32 Size;           /**< struct size*/
    MI_U32 AvgBlkX;
    MI_U32 AvgBlkY;
    MI_U32 CurRGain;
    MI_U32 CurGGain;
    MI_U32 CurBGain;
    void *avgs; //ISP_AWB_SAMPLE
}__attribute__((packed, aligned(1))) CusAWBInfo_t, CusAWBInput_t;

/*! @brief AWB algorithm result*/
typedef struct
{
    MI_U32 Size;       /**< struct size*/
    MI_U32 Change;    /**< if true, apply this result to hw register*/
    MI_U32 R_gain;     /**< AWB gain for R channel*/
    MI_U32 G_gain;     /**< AWB gain for G channel*/
    MI_U32 B_gain;     /**< AWB gain for B channel*/
    MI_U32 ColorTmp;   /**< Return color temperature*/
}CusAWBResult_t, CusAwbOutput_t;;

typedef enum __attribute__ ((aligned (1)))
{
    SS_AE_16x24 = 0,
    SS_AE_32x24,
    SS_AE_64x48,
    SS_AE_64x45,
    SS_AE_128x80,
    SS_AE_128x90,
    SS_AE_32x32
} MS_CUST_AE_WIN_BLOCK_NUM_TYPE_e;

typedef struct {
    MI_U16 u2Stawin_x_offset;
    MI_U16 u2Stawin_x_size;
    MI_U16 u2Stawin_y_offset;
    MI_U16 u2Stawin_y_size;
    MI_U16 u2WinIdx;
} CusAEHistWin_t;

typedef struct
{
    MI_U32 SizeX;
    MI_U32 SizeY;
    MI_U32 IncRatio;
}CusAWBSample_t;

typedef struct
{
    MI_U8 iir_1[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 iir_2[5*16];  //[5]: iir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 luma[4*16];   //[4]: luma 32bit, use 4*u8 datatype, [16]: 16wins
    MI_U8 fir_v[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 fir_h[5*16];  //[5]: fir 35bit, use 5*u8 datatype,     [16]: 16wins
    MI_U8 ysat[3*16];   //[3]: ysat 22bit, use 3*u8 datatype,  [16]: 16wins
} AF_STATS_PARAM_t;

typedef struct
{
    AF_STATS_PARAM_t stParaAPI[16];
} CusAFStats_t;

typedef struct AF_WINDOW_PARAM_s
{
    MI_U32 u32StartX;                                           /*range : 0~1023*/
    MI_U32 u32StartY;                                           /*range : 0~1023*/
    MI_U32 u32EndX;                                             /*range : 0~1023*/
    MI_U32 u32EndY;                                             /*range : 0~1023*/
} AF_WINDOW_PARAM_t;

typedef struct
{
    MI_U8 u8WindowIndex;
    AF_WINDOW_PARAM_t stParaAPI;
} CusAFWin_t;

typedef struct
{
    MI_U16 u16IIR1_a0;
    MI_U16 u16IIR1_a1;
    MI_U16 u16IIR1_a2;
    MI_U16 u16IIR1_b1;
    MI_U16 u16IIR1_b2;
    MI_U16 u16IIR1_1st_low_clip;
    MI_U16 u16IIR1_1st_high_clip;
    MI_U16 u16IIR1_2nd_low_clip;
    MI_U16 u16IIR1_2nd_high_clip;
    MI_U16 u16IIR2_a0;
    MI_U16 u16IIR2_a1;
    MI_U16 u16IIR2_a2;
    MI_U16 u16IIR2_b1;
    MI_U16 u16IIR2_b2;
    MI_U16 u16IIR2_1st_low_clip;
    MI_U16 u16IIR2_1st_high_clip;
    MI_U16 u16IIR2_2nd_low_clip;
    MI_U16 u16IIR2_2nd_high_clip;
} CusAFFilter_t;

typedef struct
{
    MI_BOOL bSobelYSatEn;
    MI_U16  u16SobelYThd;

    MI_BOOL bIIRSquareAccEn;
    MI_BOOL bSobelSquareAccEn;

    MI_U16  u16IIR1Thd;
    MI_U16  u16IIR2Thd;
    MI_U16  u16SobelHThd;
    MI_U16  u16SobelVThd;
    MI_U8   u8AFTblX[12];
    MI_U16  u16AFTblY[13];
} CusAFFilterSq_t;


typedef enum __attribute__ ((aligned (1)))
{
    AF_ROI_MODE_NORMAL,
    AF_ROI_MODE_MATRIX
} ISP_AF_ROI_MODE_e;

typedef struct
{
    ISP_AF_ROI_MODE_e mode;
    MI_U32 u32_vertical_block_number;
} CusAFRoiMode_t;

/* Raw store control */
typedef enum
{
    eRawStoreNode_P0HEAD = 0, /* Control by VIF, Do not use */
    eRawStoreNode_P1HEAD = 1, /* Control by VIF, Do not use */
    eRawStoreNode_P0TAIL = 2,
    eRawStoreNode_P1TAIL = 3,
    eRawStoreNode_ISPOUT = 4,
    eRawStoreNode_VDOS   = 5,
    eRawStoreNode_ISPOUT_BEFORE_YEE = 6,
    eRawStoreNode_RGBIR_IR_ONLY = 7
}CameraRawStoreNode_e;

typedef struct
{
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32Node;
    MI_U32 u32PixelDepth;
} CusImageResolution_t;

typedef struct
{
    MI_U32 u32enable;
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32physical_address;
    MI_U32 u32Node;
} CusISPOutImage_t;

typedef struct
{
    MI_U32 u32enable;
    MI_U32 u32image_width;
    MI_U32 u32image_height;
    MI_U32 u32physical_address;
    MI_U32 u32Node;
    MI_U32 u32PixelDepth;
} CusHdrRawImage_t;

/***************************** end of Cus3A ****************************/

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_DATATYPE_H_
