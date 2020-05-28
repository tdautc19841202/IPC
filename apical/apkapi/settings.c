#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/shm.h>
#include "apkapi.h"
#include "settings.h"

void ipcam_settings_load_from_buffer(IPCAMSETTINGS *settings, char *buf, int size)
{
    char str[256];
    parse_params(buf, "paired"            , str, sizeof(str)); settings->paired            = atoi(str);
    parse_params(buf, "standby"           , str, sizeof(str)); settings->standby           = atoi(str);
    parse_params(buf, "led_en"            , str, sizeof(str)); settings->led_en            = atoi(str);
    parse_params(buf, "mic_en"            , str, sizeof(str)); settings->mic_en            = atoi(str);
    parse_params(buf, "ir_en"             , str, sizeof(str)); settings->ir_en             = atoi(str);
    parse_params(buf, "hflip_en"          , str, sizeof(str)); settings->hflip_en          = atoi(str);
    parse_params(buf, "vflip_en"          , str, sizeof(str)); settings->vflip_en          = atoi(str);
    parse_params(buf, "md_en"             , str, sizeof(str)); settings->md_en             = atoi(str);
    parse_params(buf, "md_sensitivity"    , str, sizeof(str)); settings->md_sensitivity    = atoi(str);
    parse_params(buf, "md_frequence"      , str, sizeof(str)); settings->md_frequence      = atoi(str);
    parse_params(buf, "md_timeperiod"     , str, sizeof(str)); settings->md_timeperiod     = atoi(str);
    parse_params(buf, "md_freq_tab_0"     , str, sizeof(str)); settings->md_freq_tab[0]    = atoi(str);
    parse_params(buf, "md_freq_tab_1"     , str, sizeof(str)); settings->md_freq_tab[1]    = atoi(str);
    parse_params(buf, "md_freq_tab_2"     , str, sizeof(str)); settings->md_freq_tab[2]    = atoi(str);
    parse_params(buf, "rec_type"          , str, sizeof(str)); settings->rec_type          = atoi(str);
    parse_params(buf, "rawrec_en"         , str, sizeof(str)); settings->rawrec_en         = atoi(str);
    parse_params(buf, "iqserv_en"         , str, sizeof(str)); settings->iqserv_en         = atoi(str);
    parse_params(buf, "ethnet_en"         , str, sizeof(str)); settings->ethnet_en         = atoi(str);
    parse_params(buf, "sensor_frame_rate" , str, sizeof(str)); settings->sensor_frame_rate = atoi(str);
    parse_params(buf, "main_video_width"  , str, sizeof(str)); settings->main_video_width  = atoi(str);
    parse_params(buf, "main_video_height" , str, sizeof(str)); settings->main_video_height = atoi(str);
    parse_params(buf, "main_video_frate"  , str, sizeof(str)); settings->main_video_frate  = atoi(str);
    parse_params(buf, "main_video_brate"  , str, sizeof(str)); settings->main_video_brate  = atoi(str);
    parse_params(buf, "main_venc_type"    , str, sizeof(str)); settings->main_venc_type    = atoi(str);
    parse_params(buf, "sub_video_width"   , str, sizeof(str)); settings->sub_video_width   = atoi(str);
    parse_params(buf, "sub_video_height"  , str, sizeof(str)); settings->sub_video_height  = atoi(str);
    parse_params(buf, "sub_video_frate"   , str, sizeof(str)); settings->sub_video_frate   = atoi(str);
    parse_params(buf, "sub_video_brate"   , str, sizeof(str)); settings->sub_video_brate   = atoi(str);
    parse_params(buf, "sub_venc_type"     , str, sizeof(str)); settings->sub_venc_type     = atoi(str);
    parse_params(buf, "lsen_thresholdl"   , str, sizeof(str)); settings->lsen_thresholdl   = atoi(str);
    parse_params(buf, "lsen_thresholdh"   , str, sizeof(str)); settings->lsen_thresholdh   = atoi(str);
    parse_params(buf, "watermark_visible" , str, sizeof(str)); settings->watermark_visible = atoi(str);
    parse_params(buf, "watermark_posx"    , str, sizeof(str)); settings->watermark_posx    = atoi(str);
    parse_params(buf, "watermark_posy"    , str, sizeof(str)); settings->watermark_posy    = atoi(str);
    parse_params(buf, "watermark_timezone", str, sizeof(str)); settings->watermark_timezone= atoi(str);
    parse_params(buf, "exposure_time"     , str, sizeof(str)); settings->exposure_time     = atoi(str);
    parse_params(buf, "crash_reboot_flag" , str, sizeof(str)); settings->crash_reboot_flag = atoi(str);
    parse_params(buf, "crash_reboot_count", str, sizeof(str)); settings->crash_reboot_count= atoi(str);
    parse_params(buf, "light_mode"        , str, sizeof(str)); settings->light_mode        = atoi(str);
    parse_params(buf, "light_brightness"  , str, sizeof(str)); settings->light_brightness  = atoi(str);
    parse_params(buf, "light_sensitivity" , str, sizeof(str)); settings->light_sensitivity = atoi(str);
    parse_params(buf, "light_turnontime"  , str, sizeof(str)); settings->light_turnontime  = atoi(str);
    parse_params(buf, "light_timerperoid" , str, sizeof(str)); settings->light_timerperoid = atoi(str);
    parse_params(buf, "tuya_blub_on_off"  , str, sizeof(str)); settings->tuya_blub_on_off  = atoi(str);
    parse_params(buf, "tuya_pir_on_off"   , str, sizeof(str)); settings->tuya_pir_on_off   = atoi(str);
    parse_params(buf, "signature", settings->signature, sizeof(settings->signature));
    parse_params(buf, "ft_mode"  , settings->ft_mode  , sizeof(settings->ft_mode  ));
    parse_params(buf, "ft_uid"   , settings->ft_uid   , sizeof(settings->ft_uid   ));
}

static void ipcam_settings_load_from_file(IPCAMSETTINGS *settings)
{
    FILE *fp    = fopen("/customer/data/ipcam/ipcam.ini", "rb");
    int   fsize = 0;
    char *buf   = NULL;

    if (fp) {
        int fd = fileno(fp);
        flock(fd, LOCK_EX);
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buf = malloc(fsize);
        if (buf) {
            fread(buf, fsize, 1, fp);
            ipcam_settings_load_from_buffer(settings, buf, fsize);
            free (buf);
        }
        flock(fd, LOCK_UN);
        fclose(fp);
    }

    if (!fp || strcmp(settings->signature, "ipcam_settings") != 0) {
        memset(settings, 0, sizeof(IPCAMSETTINGS));
        strncpy(settings->signature, "ipcam_settings", sizeof(settings->signature));
        settings->led_en            = 1;
        settings->mic_en            = 1;
        settings->ir_en             = 0;
        settings->md_sensitivity    = 1;
        settings->md_frequence      = 1;
        settings->md_timeperiod     = (0 << 8) | (24 << 0);
        settings->rec_type          = 1;
        settings->watermark_visible = 1;
        settings->watermark_timezone= 8 * 60 * 60;
        settings->tuya_blub_on_off  = 0;
        settings->tuya_pir_on_off   = 0;
    }

    // handle default value
    if (!settings->sensor_frame_rate ) settings->sensor_frame_rate = 15;
    if (!settings->md_freq_tab[0]    ) settings->md_freq_tab[0]    = 300*1000;
    if (!settings->md_freq_tab[1]    ) settings->md_freq_tab[1]    = 120*1000;
    if (!settings->md_freq_tab[2]    ) settings->md_freq_tab[2]    = 60 *1000;
    if (!settings->main_video_width  ) settings->main_video_width  = 1920;
    if (!settings->main_video_height ) settings->main_video_height = 1080;
    if (!settings->main_video_frate  ) settings->main_video_frate  = 15;
    if (!settings->main_video_brate  ) settings->main_video_brate  = 2000000;
    if (!settings->sub_video_width   ) settings->sub_video_width   = 704;
    if (!settings->sub_video_height  ) settings->sub_video_height  = 396;
    if (!settings->sub_video_frate   ) settings->sub_video_frate   = 15;
    if (!settings->sub_video_brate   ) settings->sub_video_brate   = 512000;
    if (!settings->lsen_thresholdl   ) settings->lsen_thresholdl   = 10;
    if (!settings->lsen_thresholdh   ) settings->lsen_thresholdh   = 96;
    if (!settings->watermark_posx    ) settings->watermark_posx    = 10;
    if (!settings->watermark_posy    ) settings->watermark_posy    = 10;
    if (!settings->exposure_time     ) settings->exposure_time     = 20000;
    if (!settings->light_brightness  ) settings->light_brightness  = 10;
    if (!settings->light_turnontime  ) settings->light_turnontime  = 25;
    if (settings->md_sensitivity    < 0 || settings->md_sensitivity    > 2) settings->md_sensitivity    = 0;
    if (settings->md_frequence      < 0 || settings->md_frequence      > 2) settings->md_frequence      = 0;
    if (settings->light_mode        < 1 || settings->light_mode        > 4) settings->light_mode        = 1;
    if (settings->light_sensitivity < 0 || settings->light_sensitivity > 2) settings->light_sensitivity = 0;
    if (strcmp(settings->ft_mode, "") != 0) settings->led_en = settings->mic_en = 1,settings->ir_en = 0;
}

static void ipcam_settings_save_to_file(IPCAMSETTINGS *settings)
{
    FILE *fp = fopen("/customer/data/ipcam/ipcam.ini", "wb");
    if (fp) {
        int fd = fileno(fp);
        flock(fd, LOCK_EX);
        fprintf(fp,
            "signature         = %s\n"
            "ft_mode           = %s\n"
            "ft_uid            = %s\n"
            "paired            = %d\n"
            "standby           = %d\n"
            "led_en            = %d\n"
            "mic_en            = %d\n"
            "ir_en             = %d\n"
            "hflip_en          = %d\n"
            "vflip_en          = %d\n"
            "md_en             = %d\n"
            "md_sensitivity    = %d\n"
            "md_frequence      = %d\n"
            "md_timeperiod     = %d\n"
            "md_freq_tab_0     = %d\n"
            "md_freq_tab_1     = %d\n"
            "md_freq_tab_2     = %d\n"
            "rec_type          = %d\n"
            "rawrec_en         = %d\n"
            "iqserv_en         = %d\n"
            "ethnet_en         = %d\n"
            "sensor_frame_rate = %d\n"
            "main_video_width  = %d\n"
            "main_video_height = %d\n"
            "main_video_frate  = %d\n"
            "main_video_brate  = %d\n"
            "main_venc_type    = %d\n"
            "sub_video_width   = %d\n"
            "sub_video_height  = %d\n"
            "sub_video_frate   = %d\n"
            "sub_video_brate   = %d\n"
            "sub_venc_type     = %d\n"
            "lsen_thresholdl   = %d\n"
            "lsen_thresholdh   = %d\n"
            "watermark_visible = %d\n"
            "watermark_posx    = %d\n"
            "watermark_posy    = %d\n"
            "watermark_timezone= %d\n"
            "exposure_time     = %d\n"
            "crash_reboot_flag = %d\n"
            "crash_reboot_count= %d\n"
            "light_mode        = %d\n"
            "light_brightness  = %d\n"
            "light_sensitivity = %d\n"
            "light_turnontime  = %d\n"
            "light_timerperoid = %d\n"
            "tuya_blub_on_off  = %d\n"
            "tuya_pir_on_off   = %d\n",
            settings->signature,
            settings->ft_mode,
            settings->ft_uid,
            settings->paired,
            settings->standby,
            settings->led_en,
            settings->mic_en,
            settings->ir_en,
            settings->hflip_en,
            settings->vflip_en,
            settings->md_en,
            settings->md_sensitivity,
            settings->md_frequence,
            settings->md_timeperiod,
            settings->md_freq_tab[0],
            settings->md_freq_tab[1],
            settings->md_freq_tab[2],
            settings->rec_type,
            settings->rawrec_en,
            settings->iqserv_en,
            settings->ethnet_en,
            settings->sensor_frame_rate,
            settings->main_video_width,
            settings->main_video_height,
            settings->main_video_frate,
            settings->main_video_brate,
            settings->main_venc_type,
            settings->sub_video_width,
            settings->sub_video_height,
            settings->sub_video_frate,
            settings->sub_video_brate,
            settings->sub_venc_type,
            settings->lsen_thresholdl,
            settings->lsen_thresholdh,
            settings->watermark_visible,
            settings->watermark_posx,
            settings->watermark_posy,
            settings->watermark_timezone,
            settings->exposure_time,
            settings->crash_reboot_flag,
            settings->crash_reboot_count,
            settings->light_mode,
            settings->light_brightness,
            settings->light_sensitivity,
            settings->light_turnontime,
            settings->light_timerperoid,
            settings->tuya_blub_on_off,
            settings->tuya_pir_on_off);
        fflush(fp);
        fsync (fd);
        flock (fd, LOCK_UN);
        fclose(fp);
    }
}

#define IPCAM_SETTINGS_SHMID  ('s' << 24 | 'e' << 16 | 't' << 8 | 't' << 0)
void ipcam_settings_load(IPCAMSETTINGS *settings)
{
    IPCAMSETTINGS *shmptr = NULL;
    int            shmid  = shmget((key_t)IPCAM_SETTINGS_SHMID, sizeof(IPCAMSETTINGS), 0666|IPC_CREAT);
    if (shmid != -1) {
        shmptr = shmat(shmid, NULL, 0);
        shmptr = shmptr == (void*)-1 ? NULL : shmptr;
    }
    if (!shmptr) {
        ipcam_settings_load_from_file(settings);
    } else {
        if (strcmp(shmptr->signature, "ipcam_settings") != 0) {
            ipcam_settings_load_from_file(shmptr);
        }
        memcpy(settings, shmptr, sizeof(IPCAMSETTINGS));
    }
    if (shmptr) shmdt(shmptr);
}

void ipcam_settings_save(IPCAMSETTINGS *settings, int ftf)
{
    IPCAMSETTINGS *shmptr = NULL;
    int            shmid  = shmget((key_t)IPCAM_SETTINGS_SHMID, sizeof(IPCAMSETTINGS), 0666|IPC_CREAT);
    if (shmid != -1) {
        shmptr = shmat(shmid, NULL, 0);
        shmptr = shmptr == (void*)-1 ? NULL : shmptr;
    }
    if (!shmptr) {
        ipcam_settings_save_to_file(settings);
    } else {
        if (settings) memcpy(shmptr, settings, sizeof(IPCAMSETTINGS));
        if (ftf) {
            IPCAMSETTINGS old;
            ipcam_settings_load_from_file(&old);
            if (memcmp(&old, shmptr, sizeof(IPCAMSETTINGS)) != 0) {
                ipcam_settings_save_to_file(shmptr);
            }
        }
    }
    if (shmptr) shmdt(shmptr);
}
