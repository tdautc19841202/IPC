/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#define  DRV_CAMCLK_DEBUG_C
#include "cam_os_wrapper.h"
#include "drv_camclk_st.h"
#include "drv_camclk.h"
#include "drv_camclk_Api.h"
#include "hal_camclk_if.h"
#include "camclk_dbg.h"
#define PAGE_SIZE 4096
u32 gCAMCLKDbgLvl = 0x0;
#ifdef CONFIG_CAM_CLK_SYSFS
DrvCamClkShowId_e gCAMCLKDbgShowType = DRV_CAMCLK_SHOW_CLK_ALL;
u32 u32gclkshowtempidx = 0;

void DrvCamClkDebugInit(void)
{

}
u32 DrvCamclkDebugHandlerShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE*2;
    struct CamOsListHead_t *pos;
    CAMCLK_Handler *pCfg;

    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Handler ========================\n");
    str += CamOsSnprintf(str, end - str, "Clk Handler Count: %hd\n",gu16HandlerCnt);
    str += CamOsSnprintf(str, end - str, "Name              Handler  Num  CID  Enable RoundRate  CurPar SetRate/Parent\n");
    if(gu16HandlerCnt)
    {
        CAM_OS_LIST_FOR_EACH(pos,&gstClkListHead)
        {
            pCfg = CAM_OS_LIST_ENTRY(pos,CAMCLK_Handler,stList);
            str += CamOsSnprintf(str, end - str, "%-16s %8p  %-3u  %-3u       %1hhd %-9u    %-6u %-u\n",
                pCfg->pDevName,pCfg,pCfg->u32HandlerId&0xFF,pCfg->u32ClkId,pCfg->bEnable,pCfg->u32RoundRate,pCfg->u32CurrentParent,pCfg->attribute);
        }
    }
    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Handler ========================\n");
    return (str - buf);
}
u32 DrvCamclkDebugClkShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    u32 u32idx;
    u16 u16Select;
    u8 u8bEn;
    DrvCamClkGetParent_t stCfg;
    DrvCamClkGetRate_t stRateCfg;

    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Clk ========================\n");
    str += CamOsSnprintf(str, end - str, "CId  Type  Enable  RefCnt  Select  Parent  Freq\n");
    //CamOsPrintf("======================== CamClk Debug Clk ========================\n");
    //CamOsPrintf("Clk  CId  Type  Enable  Select  Parent  Freq\n");
    for(u32idx=u32gclkshowtempidx;u32idx<HAL_CAMCLK_SRC_Id_MAX;u32idx++)
    {
        DrvCamClkImplGetHwOnOff(u32idx,&u8bEn);
        DrvCamClkImplGetHwSelect(u32idx,&u16Select);
        stCfg.u32Id = u32idx;
        DrvCamClkImplGetParent(&stCfg);
        stRateCfg.u32Id = u32idx;
        DrvCamClkImplGetRate(&stRateCfg);
        str += CamOsSnprintf(str, end - str, "%3d  %4d     %s      %hhd    %4lx   %3d    %3d.%3dM\n"
                ,u32idx,gCamClkSrcNode[u32idx].u8ClkType,(u8bEn==2) ? "NA " :(u8bEn==1) ? "ON " :"OFF",gCamClkTopCurrent[u32idx].u8RefCnt,u16Select,stCfg.u32ParentId,stRateCfg.u32Freq/1000000,(stRateCfg.u32Freq%1000000)/1000);
        //CamOsPrintf("     %3d  %4d       %1hhd     %4lx   %3d  %3dM\n"
        //        ,u32idx,gCamClkSrcNode[u32idx].u32ClkType,u8bEn,u16Select,stCfg.u32ParentId,stRateCfg.u32Freq/1000000);
        if((str - buf)+150>=PAGE_SIZE)
        {
            u32gclkshowtempidx = u32idx+1;
            break;
        }
        if(u32idx == HAL_CAMCLK_SRC_Id_MAX-1)
        {
            u32gclkshowtempidx = 0;
            break;
        }
    }
    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Clk ========================\n");


    return (str - buf);
}
void DrvCamclkDebugDebugLvlStore(const char *buf)
{
    char *str = (char *)buf;
    gCAMCLKDbgLvl = CamOsStrtol(str, NULL, 16);

}
u32 DrvCamclkDebugDebugLvlShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;

    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Message Level ========================\n");
    str += CamOsSnprintf(str, end - str, "Debug log level : %lu\n",gCAMCLKDbgLvl);
    str += CamOsSnprintf(str, end - str, "======================== CamClk Debug Message Level ========================\n");

    return (str - buf);
}
#endif
#undef DRV_CAMCLK_DEBUG_C
