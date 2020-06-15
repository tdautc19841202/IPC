/*********************************************************************************

  *Copyright(C),2015-2020,
  *TUYA
  *www.tuya.comm

  *FileName: tuya_ipc_dp_handler.c
  *
  * File Description：
  * 1. DP Point Setting and Acquisition Function API
  *
  * Developer work：
  * 1. Local configuration acquisition and update.
  * 2. Set local IPC attributes, such as picture flip, time watermarking, etc.
  *    If the function is not supported, leave the function blank.
  *
**********************************************************************************/
#include "tuya_ipc_dp_utils.h"
#include "tuya_ipc_dp_handler.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_sdcard.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "apkapi.h"
#include "settings.h"

#ifdef TUYA_DP_SLEEP_MODE
VOID IPC_APP_set_sleep_mode(BOOL_T sleep_mode)
{
    printf("set sleep_mode:%d \r\n", sleep_mode);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.standby = sleep_mode;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

BOOL_T IPC_APP_get_sleep_mode(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.standby;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_LIGHT
VOID IPC_APP_set_light_onoff(BOOL_T light_on_off)
{
    printf("set light_on_off:%d \r\n", light_on_off);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.led_en = light_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

BOOL_T IPC_APP_get_light_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.led_en;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_FLIP
VOID IPC_APP_set_flip_onoff(BOOL_T flip_on_off)
{
    printf("set flip_on_off:%d \r\n", flip_on_off);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.hflip_en = flip_on_off;
    settings.vflip_en = flip_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

BOOL_T IPC_APP_get_flip_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.hflip_en;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_WATERMARK
VOID IPC_APP_set_watermark_onoff(BOOL_T watermark_on_off)
{
    printf("set watermark_on_off:%d \r\n", watermark_on_off);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.watermark_visible = watermark_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

BOOL_T IPC_APP_get_watermark_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.watermark_visible;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_WDR
VOID IPC_APP_set_wdr_onoff(BOOL_T wdr_on_off)
{
    printf("set wdr_on_off:%d \r\n", wdr_on_off);
    //TODO
    /* Wide Dynamic Range Model,BOOL type,true means on,false means off */

}

BOOL_T IPC_APP_get_wdr_onoff(VOID)
{
    return 0;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_NIGHT_MODE
STATIC CHAR_T s_night_mode[4] = {0};//for demo
VOID IPC_APP_set_night_mode(CHAR_T *p_night_mode)
{//0-automatic 1-off 2-on
    printf("set night_mode:%s \r\n", p_night_mode);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.ir_en = atoi(p_night_mode);
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

CHAR_T *IPC_APP_get_night_mode(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    snprintf(s_night_mode, sizeof(s_night_mode), "%d", settings.ir_en);
    return s_night_mode;
}
#endif


//------------------------------------------

#ifdef TUYA_DP_ALARM_FUNCTION
VOID IPC_APP_set_alarm_function_onoff(BOOL_T alarm_on_off)
{
    printf("set alarm_on_off:%d \r\n", alarm_on_off);
    /* motion detection alarm switch,BOOL type,true means on,false means off.
     * This feature has been implemented, and developers can make local configuration settings and properties.*/
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.md_en = alarm_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

BOOL_T IPC_APP_get_alarm_function_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.md_en;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_ALARM_SENSITIVITY
STATIC CHAR_T s_alarm_sensitivity[4] = {0};//for demo
VOID IPC_APP_set_alarm_sensitivity(CHAR_T *p_sensitivity)
{
    printf("set alarm_sensitivity:%s \r\n", p_sensitivity);
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.md_sensitivity = atoi(p_sensitivity);
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

CHAR_T *IPC_APP_get_alarm_sensitivity(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    snprintf(s_alarm_sensitivity, sizeof(s_alarm_sensitivity), "%d", settings.md_sensitivity);
    return s_alarm_sensitivity;
}
#endif

#ifdef TUYA_DP_ALARM_ZONE_ENABLE
VOID IPC_APP_set_alarm_zone_onoff(BOOL_T alarm_zone_on_off)
{
    /* Motion detection area setting switch,BOOL type,true means on,false is off*/
    printf("set alarm_zone_onoff:%d \r\n", alarm_zone_on_off);
}

BOOL_T IPC_APP_get_alarm_zone_onoff(VOID)
{
    return 0;
}
#endif


#ifdef TUYA_DP_ALARM_ZONE_DRAW

#define MAX_ALARM_ZONE_NUM      (6)     //Supports the maximum number of detection areas
//Detection area structure
typedef struct {
    char pointX;    //Starting point x  [0-100]
    char pointY;    //Starting point Y  [0-100]
    char width;     //width    [0-100]
    char height;    //height    [0-100]
} ALARM_ZONE_T;

typedef struct {
    int iZoneNum;   //Number of detection areas
    ALARM_ZONE_T alarmZone[MAX_ALARM_ZONE_NUM];
} ALARM_ZONE_INFO_T;


VOID IPC_APP_set_alarm_zone_draw(cJSON * p_alarm_zone)
{
    return;
}

static char s_alarm_zone[256] = {0};
char * IPC_APP_get_alarm_zone_draw(VOID)
{
    return s_alarm_zone;
}
#endif

//------------------------------------------

//#ifdef TUYA_DP_ALARM_INTERVAL
//STATIC CHAR_T s_alarm_interval[4] = {0};//for demo
//VOID IPC_APP_set_alarm_interval(CHAR_T *p_interval)
//{
//    printf("set alarm_interval:%s \r\n", p_interval);
//    //TODO
//    /* Motion detection alarm interval,unit is minutes,ENUM type,"1","5","10","30","60" */

//    __tuya_app_write_STR("tuya_alarm_interval", p_interval);
//}

//CHAR_T *IPC_APP_get_alarm_interval(VOID)
//{
//    /* Motion detection alarm interval,unit is minutes,ENUM type,"1","5","10","30","60" */
//    __tuya_app_read_STR("tuya_alarm_interval", s_alarm_interval, 4);
//    printf("curr alarm_intervaly:%s \r\n", s_alarm_interval);
//    return s_alarm_interval;
//}
//#endif

//------------------------------------------

#ifdef TUYA_DP_SD_STATUS_ONLY_GET
INT_T IPC_APP_get_sd_status(VOID)
{
    return tuya_ipc_sd_get_status();
}
#endif

//------------------------------------------

#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
VOID IPC_APP_get_sd_storage(UINT_T *p_total, UINT_T *p_used, UINT_T *p_empty)
{
    tuya_ipc_sd_get_capacity(p_total, p_used, p_empty);
}
#endif

//------------------------------------------

#ifdef TUYA_DP_SD_RECORD_ENABLE
VOID IPC_APP_set_sd_record_onoff(BOOL_T sd_record_on_off)
{
    printf("set sd_record_on_off:%d \r\n", sd_record_on_off);
    /* SD card recording function swith, BOOL type, true means on, false means off.
     * This function has been implemented, and developers can make local configuration settings and properties.*/
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.rec_type &= ~(1 << 1);
    settings.rec_type |=  sd_record_on_off ? (1 << 1) : 0;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
    printf("sd_record_on_off = %d\n",sd_record_on_off);
    printf("set rec_type = %d\n",!!(settings.rec_type & (1 << 1)));
}

BOOL_T IPC_APP_get_sd_record_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return !!(settings.rec_type & (1 << 1));
}
#endif

//------------------------------------------

#ifdef TUYA_DP_SD_RECORD_MODE
VOID IPC_APP_set_sd_record_mode(UINT_T sd_record_mode)
{
    printf("set sd_record_mode:%d \r\n", sd_record_mode);
    if (0 == sd_record_mode) {
         tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_EVENT);
    } else if (1 == sd_record_mode) {
         tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_ALL);
    } else if (3 == sd_record_mode) {
         tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_NONE);
    }
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    settings.rec_type &= ~(1 << 0);
    settings.rec_type |=  sd_record_mode ? (1 << 0) : 0;
    ipcam_settings_save(&settings, 1);
    send_sd_record_mode_to_tuya();
    send_msg_to_ipcam("apply_settings", NULL);
}

UINT_T IPC_APP_get_sd_record_mode(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return (settings.rec_type & (1 << 0));
}
#endif

//------------------------------------------

#ifdef TUYA_DP_SD_UMOUNT
BOOL_T IPC_APP_unmount_sd_card(VOID)
{
    BOOL_T umount_ok = TRUE;

    //TODO
    /* unmount sdcard */

    printf("unmount result:%d \r\n", umount_ok);
    return umount_ok;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_SD_FORMAT
/* -2000: SD card is being formatted, -2001: SD card formatting is abnormal, -2002: No SD card,
   -2003: SD card error. Positive number is formatting progress */
STATIC INT_T s_sd_format_progress = 0;
void *thread_sd_format(void *arg)
{
    /* First notify to app, progress 0% */
    s_sd_format_progress = 0;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    sleep(1);

    /* Stop local SD card recording and playback, progress 10%*/
    s_sd_format_progress = 10;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_NONE);
    tuya_ipc_ss_pb_stop_all();
    sleep(1);

    /* Delete the media files in the SD card, the progress is 30% */
    s_sd_format_progress = 30;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    sleep(1);

    /* Perform SD card formatting operation */
    tuya_ipc_sd_format();

    s_sd_format_progress = 80;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    tuya_ipc_ss_set_write_mode(tuya_ipc_sd_get_mode_config());

    sleep(1);
    IPC_APP_report_sd_storage();
    /* progress 100% */
    s_sd_format_progress = 100;
    IPC_APP_report_sd_format_status(s_sd_format_progress);

    pthread_exit(0);
}

VOID IPC_APP_format_sd_card(VOID)
{
    printf("start to format sd_card \r\n");
    /* SD card formatting.
     * The SDK has already completed the writing of some of the code,
     and the developer only needs to implement the formatting operation. */

    pthread_t sd_format_thread;
    pthread_create(&sd_format_thread, NULL, thread_sd_format, NULL);
    pthread_detach(sd_format_thread);
}
#endif

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
INT_T IPC_APP_get_sd_format_status(VOID)
{
    return s_sd_format_progress;
}
#endif

#ifdef TUYA_DP_DEVICE_RESTART
VOID IPC_APP_DP_device_reboot(VOID)
{
    CHAR_T * value = "1";
    printf("reboot\n");
    send_msg_to_ipcam("reboot",value);
}
#endif
//------------------------------------------

#ifdef TUYA_DP_PTZ_CONTROL
VOID IPC_APP_ptz_start_move(CHAR_T *p_direction)
{
    printf("ptz start move:%s \r\n", p_direction);
    //0-up, 1-upper right, 2-right, 3-lower right, 4-down, 5-down left, 6-left, 7-top left
    send_msg_to_ipcam("motor", p_direction);
}
#endif

#ifdef TUYA_DP_PTZ_STOP
VOID IPC_APP_ptz_stop_move(VOID)
{
    printf("ptz stop move \r\n");
    /* PTZ rotation stops */
    send_msg_to_ipcam("motor", "-1");
}
#endif

#ifdef TUYA_DP_PTZ_CHECK
void IPC_APP_ptz_check(VOID)
{
    printf("ptz check \r\n");
}
#endif

#ifdef TUYA_DP_TRACK_ENABLE
void IPC_APP_track_enable(BOOL_T track_enable)
{
    printf("track_enable %d\r\n",track_enable);
}

BOOL_T IPC_APP_get_track_enable(void)
{
    char track_enable = 0;
    //the value you get yourself
    return (BOOL_T)track_enable;
}
#endif

#ifdef TUYA_DP_HUM_FILTER
void IPC_APP_human_filter(BOOL_T filter_enable)
{
    printf("filter_enable %d\r\n",filter_enable);
    return;
}
#endif

#ifdef TUYA_DP_PATROL_MODE
void IPC_APP_set_patrol_mode(BOOL_T patrol_mode)
{
    printf("patrol_mode %d\r\n",patrol_mode);
    return;
}

char IPC_APP_get_patrol_mode(void)
{
    char patrol_mode = 0;
    //the value you get yourself
    return patrol_mode;
}
#endif

#ifdef TUYA_DP_PATROL_SWITCH
void IPC_APP_set_patrol_switch(BOOL_T patrol_switch)
{
    printf("patrol_switch %d\r\n",patrol_switch);
    return;
}

BOOL_T IPC_APP_get_patrol_switch(void)
{
    char patrol_switch = 0;
    //the value you get yourself
    return (BOOL_T)patrol_switch;
}

void IPC_APP_ptz_preset_reset(S_PRESET_CFG *preset_cfg)
{
    /*Synchronize data from server*/
    return;
}
#endif

#ifdef TUYA_DP_PATROL_TMODE
void IPC_APP_set_patrol_tmode(BOOL_T patrol_tmode)
{
    printf("patrol_tmode %d\r\n",patrol_tmode);
    return;
}

char IPC_APP_get_patrol_tmode(void)
{
    char patrol_tmode = 0;
    //the value you get yourself
    return patrol_tmode;
}
#endif

#ifdef TUYA_DP_PATROL_TIME
void IPC_APP_set_patrol_time(cJSON * p_patrol_time)
{
    //set your patrol_time
    return;
}
#endif

#ifdef TUYA_DP_PRESET_SET
void IPC_APP_set_preset(cJSON * p_preset_param)
{
    //preset add ,preset del, preset go
    return;
}
#endif

#ifdef TUYA_DP_PATROL_STATE
void IPC_APP_patrol_state(int *patrol_state)
{
    //printf("patrol_state %d\r\n",atoi(patrol_state));
    //return your patrol_state
    return;
}
#endif

//------------------------------------------

#ifdef TUYA_DP_BLUB_SWITCH
VOID IPC_APP_set_blub_onoff(BOOL_T blub_on_off)
{
    IPCAMSETTINGS settings;

    printf("blub_on_off = %d\n",blub_on_off);
    ipcam_settings_load(&settings);
    settings.light_mode = 0x03;
    settings.tuya_blub_on_off = blub_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
    return;
}

BOOL_T IPC_APP_get_blub_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return (BOOL_T) settings.tuya_blub_on_off;
}
#endif

#ifdef TUYA_DP_LIGHT_SENSITIVITY
STATIC CHAR_T s_light_sensitivity[2] = {0};
VOID IPC_APP_set_light_senditivity(CHAR_T *light_sensitivity)
{
    IPCAMSETTINGS settings;
    
    printf("light_sensitivity = %d\n",atoi(light_sensitivity));
    ipcam_settings_load(&settings);
    settings.light_mode = 0x04;
    settings.light_sensitivity = atoi(light_sensitivity);
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
}

CHAR_T *IPC_APP_get_light_senditivity(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    snprintf(s_light_sensitivity, sizeof(s_light_sensitivity), "%d", settings.light_sensitivity);
    return s_light_sensitivity;
}
#endif

#ifdef TUYA_DP_LIGHT_PIR_SWITCH
VOID IPC_APP_set_pir_onoff(BOOL_T pir_on_off)
{
    IPCAMSETTINGS settings;
    
    printf("pir_on_off = %d\n",pir_on_off);
    ipcam_settings_load(&settings);
    settings.light_mode = 0x01;
    settings.tuya_pir_on_off = pir_on_off;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
    return;
}

/*
 *  PIR打开表示目前是云动的模式1(对应自动模式)
*/
BOOL_T IPC_APP_get_pir_onoff(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return (BOOL_T)settings.tuya_pir_on_off;
}
#endif

#ifdef TUYA_DP_LIGHT_PIR_TIME
VOID IPC_APP_set_pir_time(INT_T value)
{
    IPCAMSETTINGS settings;
    
    printf("pir_time = %d\n",value);
    ipcam_settings_load(&settings);
    settings.light_mode = 0x01;
    settings.light_turnontime = value;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
    return;
}

INT_T IPC_APP_get_pir_time(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return (INT_T)settings.light_turnontime;
}
#endif

#ifdef TUYA_DP_LIGHT_BRIGHTNESS
VOID IPC_APP_set_light_brightness(INT_T value)
{
    IPCAMSETTINGS settings;
    
    printf("light_brightness = %d\n",value);
    ipcam_settings_load(&settings);
    settings.light_mode = 0x02;
    settings.light_brightness = value;
    ipcam_settings_save(&settings, 1);
    send_msg_to_ipcam("apply_settings", NULL);
    return;
}

INT_T IPC_APP_get_light_brightness(VOID)
{
    IPCAMSETTINGS settings;
    ipcam_settings_load(&settings);
    return settings.light_brightness;
}

#endif


#ifdef TUYA_DP_ELECTRICITY
INT_T IPC_APP_get_battery_percent(VOID)
{
    //TODO
    /* battery power percentage VALUE type,[0-100] */
    return 100;
}
#endif

#ifdef TUYA_DP_POWERMODE
CHAR_T *IPC_APP_get_power_mode(VOID)
{
    //TODO
    /* Power supply mode, ENUM type,
    "0" is the battery power supply state, "1" is the plug-in power supply state (or battery charging state) */
    return "1";
}
#endif
