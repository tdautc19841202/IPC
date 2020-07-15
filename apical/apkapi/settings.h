#ifndef __IPCAM_SETTINGS_H__
#define __IPCAM_SETTINGS_H__

#include <stdint.h>

typedef struct {
    char signature[16]; // must be "ipcam_settings"
    char ft_mode  [6 ]; // "" - normal, "smt" - smt test mode, "uvc" - uvc test mode, "monly" - motor test only, "mtest" motor aging test
    char ft_uid   [16];
    int paired;
    int  standby;
    int  led_en;
    int  mic_en;
    int  ir_en;
    int  hflip_en;
    int  vflip_en;

    int  md_en;
    int  md_sensitivity;
    int  md_frequence;
    int  md_timeperiod;
    int  md_freq_tab[3];

    int  rec_type; // 0 - disable, 1 - sdcardrec motiondet, 2 - sdcardrec endless
    int  rawrec_en;
    int  iqserv_en;
    int  ethnet_en;

    int  sensor_frame_rate;
    int  main_video_width;
    int  main_video_height;
    int  main_video_frate;
    int  main_video_brate;
    int  main_venc_type;

    int  sub_video_width;
    int  sub_video_height;
    int  sub_video_frate;
    int  sub_video_brate;
    int  sub_venc_type;

    int  lsen_thresholdl;
    int  lsen_thresholdh;

    int  watermark_visible;
    int  watermark_posx;
    int  watermark_posy;
    int  watermark_timezone;
    int  exposure_time;

    int  crash_reboot_flag ;
    int  crash_reboot_count;

    int  light_mode;
    int  light_brightness;
    int  light_sensitivity;
    int  light_turnontime;
    int  light_timerperoid;
    int  tuya_blub_on_off;
    int  tuya_pir_on_off;
    
    int  soft_light_sensor_LV;
    int  soft_light_sensor_SensorGain;
    float  soft_light_sensor_AWB;
    uint32_t rsum, gsum, bsum;
    int is_night;
} IPCAMSETTINGS;

void ipcam_settings_load(IPCAMSETTINGS *settings);
void ipcam_settings_save(IPCAMSETTINGS *settings, int ftf); // ftf - flush settings to file
void ipcam_settings_load_from_buffer(IPCAMSETTINGS *settings, char *buf, int size);

#endif

