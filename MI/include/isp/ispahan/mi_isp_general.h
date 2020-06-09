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
*   mi_isp.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_ISP_GENERAL_H_
#define _MI_ISP_GENERAL_H_

#include <libcamera_private.h>
#include <libcamera_io.h>

//#include "mi_common.h"
typedef unsigned char                            MI_U8;         // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short                           MI_U16;        // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                             MI_U32;        // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long                       MI_U64;        // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                              MI_S8;         // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                             MI_S16;        // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                               MI_S32;        // 4 bytes

//#include "mi_isp.h"
#define MI_ISP_OK                       (0)
#define MI_ISP_NOT_SUPPORT              (1)
#define MI_ISP_FAILURE                  (-1)
#define MI_ISP_API_MAX_PARM_NUMBRE      (6)
#define MI_ISP_API_ISPMID_NOT_SUPPORT   (5)

//#include "mi_isp_datatype.h"
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

#ifdef __cplusplus
extern "C"
{
#endif

#define MI_ISP_MSG(args...)         IspLogInfo(args)
#define MI_ISP_DMSG(args...)        do{}while(0)
#define MI_ISP_EMSG(args...)        IspLogInfo(args)
#define MI_ISP_VMSG(args...)        do{}while(0)

#define MI_ISP_CALLOC    CamOsMemCalloc
#define MI_ISP_FREE      CamOsMemRelease

#define MI_GETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    s32Ret = MS_CAM_IspApiGet(Channel, APIFuncID, sizeof(PARAM_t), PtrData);\
    return s32Ret;\

#define MI_SETAPI(Channel, APIFuncID, PARAM_t, PtrData)\
    MI_S32 s32Ret = MI_ISP_OK;\
    MI_U32 *pData = MI_ISP_CALLOC(1, sizeof(MI_U32)+sizeof(PARAM_t));\
    pData[0] = sizeof(PARAM_t);\
    memcpy(&pData[1], PtrData, sizeof(PARAM_t));\
    s32Ret = MS_CAM_IspApiSet(Channel, APIFuncID, sizeof(PARAM_t), &pData[1]);\
    MI_ISP_FREE(pData);\
    return s32Ret;\

#define MI_CALI_SETAPI(Channel, APIFuncID, PARAM_t, DataSize, PtrData)\
    MI_U32 *pData = MI_ISP_CALLOC(1, sizeof(MI_U32)+DataSize);\
    pData[0] = DataSize;\
    memcpy(&pData[1], PtrData, DataSize);\
    s32Ret = MS_CAM_IspApiSet(Channel, APIFuncID, DataSize, &pData[1]);\
    MI_ISP_FREE(pData);\

#define MI_ISP_SET(APIFunc, PARAM_t) \
    if(ApiLen != sizeof(PARAM_t)) \
    { \
        MI_ISP_MSG("[%s][%s] APIID:%d error param \n", __FILE__, __func__, ApiId); \
        break; \
    } \
    ret = APIFunc(Channel, (PARAM_t*)pApiBuf); \

#define MI_ISP_GET(APIFunc, PARAM_t) \
    *ApiLen = sizeof(PARAM_t); \
    ret = APIFunc(Channel, (PARAM_t*)pApiBuf); \

#define MI_CALL_SETAPI(Channel, APIFunc, PARAM_t)\
    MI_S32 s32Ret = MI_ISP_OK;\
    PARAM_t *stParam = MI_ISP_CALLOC(1, sizeof(PARAM_t));\
    memcpy(stParam, param_ary[0], sizeof(PARAM_t));\
    s32Ret = APIFunc(Channel, stParam);\
    MI_ISP_DMSG("[%s][%s] param_num = %d\n", __FILE__, __FUNCTION__, param_num);\
    MI_ISP_FREE(stParam);\
    return s32Ret;\

#define MI_RESET_API(Channel, APIFunc, PARAM_t)\
    PARAM_t *stParam = MI_ISP_CALLOC(1, sizeof(PARAM_t));\
    memset(stParam, 0x00, sizeof(PARAM_t));\
    s32Ret = APIFunc(Channel, stParam);\
    MI_ISP_DMSG("[%s][%s] param_num = %d\n", __FILE__, __FUNCTION__, param_num);\
    MI_ISP_FREE(stParam);\

#ifdef __cplusplus
}   //end of extern C
#endif

#endif  //_MI_ISP_GENERAL_H_
