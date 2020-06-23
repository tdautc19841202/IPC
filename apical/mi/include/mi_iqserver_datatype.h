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
*   mi_iqserver_datatype.h
*
*   Created on: June 27, 2018
*       Author: Jeffrey Chou
*/

#ifndef _MI_IQSERVER_DATATYPE_H_
#define _MI_IQSERVER_DATATYPE_H_

#define MI_IQSERVER_OK          0
#define MI_IQSERVER_FAIL        1
#define MI_IQSERVER_UNDEFINE    2
#define MI_IQSERVER_BUFF_FULL   3

typedef enum
{
    E_MI_IQSERVER_CMD_RESERVED,
    E_MI_IQSERVER_CMD_SET_MODE = 2,
    E_MI_IQSERVER_CMD_GET_MODE,
    E_MI_IQSERVER_CMD_GET_PIC,
    E_MI_IQSERVER_CMD_SET_API,
    E_MI_IQSERVER_CMD_GET_API,
    E_MI_IQSERVER_CMD_UPLOAD_FILE,    /* client upload file to server */
    E_MI_IQSERVER_CMD_DOWNLOAD_FILE,  /* client download file from server*/
} MI_IQSERVER_CmdType_e;

typedef struct MI_IQSERVER_CmdHeader_s
{
    MI_IQSERVER_CmdType_e CmdType;
    MI_U32 CmdLen;
}MI_IQSERVER_CmdHeader_t;

typedef struct MI_IQSERVER_Buff_s
{
    MI_U8* pAddr;
    MI_U32 u32Size;
    MI_U32 u32ContentSize;
}MI_IQSERVER_Buff_t;


#endif
