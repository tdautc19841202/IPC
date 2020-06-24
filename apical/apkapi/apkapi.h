#ifndef __APKAPI_H__
#define __APKAPI_H__

#include <stdint.h>

#define SDCARD_MOUNT_PATH   "/tmp/sdcard"
#define DEVICE_NAME         "APICAL_IPCAM"
#define FIRMWARE_VERSION    "v1.0.0.0"
int32_t get_build_num(void);
extern char HW_INFO_STR[];
extern int  GPIO_LED_R   ;
extern int  GPIO_LED_G   ;
extern int  GPIO_LED_B   ;
extern int  GPIO_LED_U   ;
extern int  GPIO_WIFI_A  ;
extern int  GPIO_WIFI_B  ;
extern int  GPIO_BUTTON  ;
extern int  GPIO_SPK_PWR ;
extern int  GPIO_IR_LED  ;
extern int  GPIO_IR_CUT0 ;
extern int  GPIO_IR_CUT1 ;
extern int  GPIO_SD_DET  ;
extern int  LED_REVERSED ;
extern int  CAM_HV_FLIP  ;
extern int  CAM_IQ_FILE  ;
extern int  HW_MOTOR_TYPE;
extern int  HW_LIGHT_TYPE;
extern int  HW_SPK_VOLUME;
extern int  GET_IP_FLAG  ;
extern int  AUTO_CONNECT ;
extern int  REMOVE_DEVICE;
void apkapi_init(void);
int  file_exist (char *file);
void file_write (char *file, char *data);
int  file_read  (char *file, char *data, int len);
int  get_gpio   (int gpio);
void set_gpio   (int gpio, int val);
void set_led    (int gpio, int val);
int  get_led    (int gpio);
void set_ircut  (int en);
int  get_cpu_uid(char *uid, int size);
int  get_dev_sid(char *sid, int size);
int  get_dev_uid(char *uid, int size);
int  get_dev_sn (char *sn , int size);
int  set_dev_ids(char *mac_eth, char *mac_wlan, char *dev_sid, char *dev_uid, char *dev_sn);
int  get_dev_mac(char *wlan_mac);
int  get_dev_ip (char *dev, char *str, int len);
void get_dev_gateway_mask(char *dev, char *gw, char *mask);
int  get_wifi_signal  (void);
int  get_netdev_status(char *dev);
int  get_sdcard_status(uint32_t *status, int64_t *total, int64_t *free);
void get_sdcard_status_from_shm(uint32_t *status, int64_t *total, int64_t *free);
int  format_sdcard(void);
int set_wlan_map();
int set_wlan_mac();
// get tick count
uint32_t get_tick_count(void);
uint64_t get_time_stamp(void);

// http string decode
void http_string_decode(char *str);

// parse param from string buffer
char* parse_params(const char *str, const char *key, char *val, int len);

// proc misc functions
int  check_proc_running(char *name);
void exit_proc_running (char *name);

// string to lower case
void str2lower(char *str);

// send msg to ipcam
int send_msg_to_ipcam(char *msg, char *params);

#define SDSTATUS_SHMID (('s' << 24) | ('d' << 16) | ('s' << 8) | ('t' << 0))
typedef struct {
    #define SDSTATUS_INSERTED   (1 << 0)
    #define SDSTATUS_DETECTED   (1 << 1)
    #define SDSTATUS_MOUNTED    (1 << 2)
    #define SDSTATUS_FORMATTING (1 << 3)
    #define SDSTATUS_REQ_FORMAT (1 << 4)
    uint32_t status;
    int64_t  total;
    int64_t  available;
} SDSTATUS;

#endif
