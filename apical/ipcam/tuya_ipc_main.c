#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "tuya_cloud_base_defs.h"
#include "tuya_ipc_common.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_cloud_storage.h"
#include "tuya_ipc_system_control.h"
#include "tuya_ipc_dp_handler.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_main.h"
#include "tuya_iot_config.h"


extern int ipcam_take_snapshot(void **buf, int *size);
#define IPC_APP_STORAGE_PATH    "/customer/data/ipcam/"   //Path to save tuya sdk DB files, should be readable, writeable and storable
#define IPC_APP_UPGRADE_FILE    "/tmp/update.tar.gz" //File with path to download file during OTA
#define IPC_APP_SD_BASE_PATH    "/tmp/sdcard/"      //SD card mount directory
#define IPC_APP_PID             "szfhc68ijsgr1pul"  //Product ID of TUYA device, this is for demo only.
                                                     //Contact tuya PM/BD for official pid.
                                                     // nq67xb4kn4ywqzdm for kapianji
                                                     // wz27y3xbjkrsvc7u for yaotouji  u0tnubut7jbtrooq for 323
                                                     // hayrqfrisvjn1qev for dengban
#define IPC_APP_VERSION         "1.2.3" // Firmware version displayed on TUYA APP

IPC_MGR_INFO_S s_mgr_info    = {};
STATIC INT_T   s_sdk_inited  = 0;
STATIC INT_T   s_mqtt_status = 0;
CHAR_T s_raw_path[128]       = {};

STATIC VOID __IPC_APP_Get_Net_Status_cb(IN CONST BYTE_T stat)
{
    PR_DEBUG("Net status change to:%d", stat);
    switch (stat)
    {
        #if defined(WIFI_GW) && (WIFI_GW==1)
        case STAT_CLOUD_CONN:       // for wifi ipc
        #endif
        #if defined(WIFI_GW) && (WIFI_GW==0)
        case GB_STAT_CLOUD_CONN:    // for wired ipc
        #endif
        {
            IPC_APP_Notify_LED_Sound_Status_CB(IPC_MQTT_ONLINE);
            PR_DEBUG("mqtt is online\r\n");
            s_mqtt_status = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

OPERATE_RET IPC_APP_Init_SDK(WIFI_INIT_MODE_E init_mode, CHAR_T *uuid, CHAR_T *key, CHAR_T *p_token)
{
    PR_DEBUG("SDK Version:%s\r\n", tuya_ipc_get_sdk_info());

    memset(&s_mgr_info, 0, sizeof(IPC_MGR_INFO_S));
    strcpy(s_mgr_info.storage_path, IPC_APP_STORAGE_PATH);
    strcpy(s_mgr_info.upgrade_file_path, IPC_APP_UPGRADE_FILE);
    strcpy(s_mgr_info.sd_base_path, IPC_APP_SD_BASE_PATH);
    strcpy(s_mgr_info.product_key, IPC_APP_PID);
    strcpy(s_mgr_info.uuid, uuid);
    strcpy(s_mgr_info.auth_key, key);
    strcpy(s_mgr_info.dev_sw_version, IPC_APP_VERSION);
    s_mgr_info.max_p2p_user = 5; //TUYA P2P supports 5 users at the same time, including live preview and playback

    PR_DEBUG("Init Value.storage_path %s", s_mgr_info.storage_path);
    PR_DEBUG("Init Value.upgrade_file_path %s", s_mgr_info.upgrade_file_path);
    PR_DEBUG("Init Value.sd_base_path %s", s_mgr_info.sd_base_path);
    PR_DEBUG("Init Value.product_key %s", s_mgr_info.product_key);
    PR_DEBUG("Init Value.uuid %s", s_mgr_info.uuid);
    PR_DEBUG("Init Value.auth_key %s", s_mgr_info.auth_key);
    PR_DEBUG("Init Value.p2p_id %s", s_mgr_info.p2p_id);
    PR_DEBUG("Init Value.dev_sw_version %s", s_mgr_info.dev_sw_version);
    PR_DEBUG("Init Value.max_p2p_user %u", s_mgr_info.max_p2p_user);

    IPC_APP_Set_Media_Info();
    TUYA_APP_Init_Ring_Buffer();
    IPC_APP_Notify_LED_Sound_Status_CB(IPC_BOOTUP_FINISH);

    TUYA_IPC_ENV_VAR_S env = {};
    strcpy(env.storage_path, s_mgr_info.storage_path);
    strcpy(env.product_key,s_mgr_info.product_key);
    strcpy(env.uuid, s_mgr_info.uuid);
    strcpy(env.auth_key, s_mgr_info.auth_key);
    strcpy(env.dev_sw_version, s_mgr_info.dev_sw_version);
    strcpy(env.dev_serial_num, "tuya_ipc");
    env.dev_obj_dp_cb = IPC_APP_handle_dp_cmd_objs;
    env.dev_dp_query_cb = IPC_APP_handle_dp_query_objs;
    env.status_changed_cb = __IPC_APP_Get_Net_Status_cb;
    env.gw_ug_cb = IPC_APP_Upgrade_Inform_cb;
    env.gw_rst_cb = IPC_APP_Reset_System_CB;
    env.gw_restart_cb = IPC_APP_Restart_Process_CB;
    env.mem_save_mode = FALSE;
    tuya_ipc_init_sdk(&env);
    tuya_ipc_set_log_attr(0, NULL);
    tuya_ipc_start_sdk(init_mode, p_token);
    s_sdk_inited = 1;
    return OPRT_OK;
}

int tuya_ipc_main(char *uuid, char *key, char *token,int *exit_fun)
{
#if defined(WIFI_GW) && (WIFI_GW==0)
    mode = WIFI_INIT_NULL;
#endif
    /* Init SDK */
    IPC_APP_Init_SDK(WIFI_INIT_AUTO, uuid, key, token);

    /* whether SDK is connected to MQTT */
    while (s_mqtt_status != 1) {
        sleep(1);
        if(*exit_fun)return -1;
    }

    /* Enable p2p transfer */
    TUYA_APP_Enable_P2PTransfer(s_mgr_info.max_p2p_user);

    /* Upload all local configuration item (DP) status when MQTT connection is successful */
    IPC_APP_upload_all_status();

    TUYA_APP_Init_Stream_Storage(s_mgr_info.sd_base_path);
    TUYA_APP_Enable_CloudStorage();
//  TUYA_APP_Enable_EchoShow_Chromecast();
//  TUYA_APP_Enable_AI_Detect();

    while (1) {
        /* At least one system time synchronization after networking */
        if (IPC_APP_Sync_Utc_Time() == OPRT_OK) break;
        if(*exit_fun)return -1;
        sleep(1);
    }

    return 0;
}

static int parse_h264_nalu_header(unsigned char *data, int len, int *hdrsize)
{
    int  i;
    for (i=0; i<4 && i<len && !data[i]; i++);
    if (i < 2 || i == 4 || data[i] != 0x01 || ++i >= len) {
        printf("failed to find h264 frame data[%d] = %02x !\n", i, data[i]);
        return -1;
    }
    *hdrsize = i;
    return data[i] & 0x1f;
}

static int is_h264_key_frame(unsigned char *data, int len)
{
    int i, last, type, hdrsize, key = 0;

#if 0
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[0 ], data[1 ], data[2 ], data[3 ], data[4 ], data[5 ], data[6 ], data[7 ]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[8 ], data[9 ], data[10], data[11], data[12], data[13], data[14], data[15]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[16], data[17], data[18], data[19], data[20], data[21], data[22], data[23]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[24], data[25], data[26], data[27], data[28], data[29], data[30], data[31]);
#endif

    type = parse_h264_nalu_header(data, len, &hdrsize);
    if (type == -1) return key;
    data += hdrsize;
    len  -= hdrsize;

    if (type == 7) {
        for (i=0,last=1; i+1<len && (last || data[i] || data[i+1]); last=data[i],i++);
        data += i;
        len  -= i;

        type = parse_h264_nalu_header(data, len, &hdrsize);
        if (type == -1) return key;
        data += hdrsize;
        len  -= hdrsize;

        if (type == 8) {
            for (i=0,last=1; i<len && (last || data[i]); last=data[i],i++);
            data += i;
            len  -= i;
        } else {
            printf("not pps data !\n");
            return key;
        }

        type = parse_h264_nalu_header(data, len, &hdrsize);
        if (type == -1) return key;
        data += hdrsize;
        len  -= hdrsize;
        key   = 1;
    }

    return key;
}

void tuya_audio(unsigned char *buf, int len)
{
    MEDIA_FRAME_S frame = {};
    if (!s_sdk_inited) return;
    frame.type  = E_AUDIO_FRAME;
    frame.size  = len;
    frame.p_buf = buf;
    TUYA_APP_Put_Frame(E_CHANNEL_AUDIO, &frame);
}

void tuya_video(unsigned char *buf, int len, int sub)
{
    MEDIA_FRAME_S frame = {};
    if (!s_sdk_inited) return;
    frame.type  = is_h264_key_frame(buf, len) ? E_VIDEO_I_FRAME : E_VIDEO_PB_FRAME;
    frame.size  = len;
    frame.p_buf = buf;
    TUYA_APP_Put_Frame(!sub ? E_CHANNEL_VIDEO_MAIN : E_CHANNEL_VIDEO_SUB, &frame);
}

static uint32_t last_motion_report = 0;
void tuya_motion_event(int motion, uint32_t tick)
{
    void *snapshot_buf;
    int   snapshot_len;
    if (s_mqtt_status == 0) return;
    if (motion > 0 && last_motion_report == 0) {
        last_motion_report = tick;
        tuya_ipc_ss_start_event();  // 开始录像
        ipcam_take_snapshot(&snapshot_buf, &snapshot_len);  // 获取照片
        tuya_ipc_cloud_storage_event_start(snapshot_buf, snapshot_len, EVENT_TYPE_MOTION_DETECT);
        if (IPC_APP_get_alarm_function_onoff()) {  // 允许移动侦测
            tuya_ipc_notify_motion_detect(snapshot_buf, snapshot_len, NOTIFICATION_CONTENT_JPEG);
        }
    } else if (tick - last_motion_report > 30 * 1000) {
        if (motion == 0) {
            last_motion_report = 0;
            tuya_ipc_ss_stop_event(); // 停止录像
            tuya_ipc_cloud_storage_event_stop();
        } else {
            if (SS_WRITE_MODE_EVENT == tuya_ipc_ss_get_write_mode() && E_STORAGE_STOP == tuya_ipc_ss_get_status()) {
                tuya_ipc_ss_start_event();
            }
            if (  ClOUD_STORAGE_TYPE_EVENT == tuya_ipc_cloud_storage_get_store_mode()
               && tuya_ipc_cloud_storage_get_event_status(EVENT_TYPE_MOTION_DETECT) == EVENT_NONE) {
                ipcam_take_snapshot(&snapshot_buf, &snapshot_len);
                tuya_ipc_cloud_storage_event_start(snapshot_buf, snapshot_len, EVENT_TYPE_MOTION_DETECT);
            }
        }
    }
}

int get_mqtt_status(void)
{
    return tuya_ipc_get_mqtt_status();  //0表示mqtt未连接，1表示已连接，失败返回错误码
}


