/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_ipc_qrcode_demo
**********************************************************************************/
#include "tuya_ipc_api.h"
#include "tuya_ipc_qrcode_proc.h"

#if defined(WIFI_GW) && (WIFI_GW==1)

extern char g_zbar_scan_str[];
STATIC CHAR_T* __tuya_linux_get_snap_qrcode(VOID)
{
    //Developers need to parse QR code information from cameras
    //a typical string is {"s":"ssidxxxx","p":"password","t":"token frome tuya cloud"}
    //using 'Tuya_Ipc_QRCode_Enhance' when hard to parse the results
    return g_zbar_scan_str[0] ? g_zbar_scan_str : NULL;
}

int s_enable_qrcode = 1;
void *thread_qrcode(void *arg)
{
    printf("Qrcode Thread start\r\n");
    while(s_enable_qrcode)
    {
        usleep(1000*1000);
        char *pStr = __tuya_linux_get_snap_qrcode();
        if(pStr)
        {
            printf("get string from qrcode %s\r\n",pStr);
            OPERATE_RET ret = tuya_ipc_direct_connect(pStr, TUYA_IPC_DIRECT_CONNECT_QRCODE);
            if(ret == OPRT_OK)
            {
                printf("register to tuya cloud via qrcode success\r\n");
                break;
            }
        }
    }

    printf("Qrcode Proc Finish\r\n");
    return (void *)0;
}
#endif

