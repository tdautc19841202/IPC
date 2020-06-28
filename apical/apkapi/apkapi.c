#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <mdrv_msys_io_st.h>
#include <mdrv_msys_io.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <sys/mount.h>
#include <sys/shm.h>
#include "libmtd.h"
#include "apkapi.h"

char HW_INFO_STR[128];
int GPIO_LED_R   = 76;
int GPIO_LED_G   = 77;
int GPIO_LED_B   = 45;
int GPIO_LED_U   = 77;
int GPIO_WIFI    = 14;
int GPIO_BUTTON  = 66;
int GPIO_SPK_PWR = 15;
int GPIO_IR_LED  = 61;
int GPIO_IR_CUT0 = 78;
int GPIO_IR_CUT1 = 79;
int GPIO_SD_DET  = 60;
int LED_REVERSED =  0;
int CAM_HV_FLIP  =  3;
int CAM_IQ_FILE  = -1;
int HW_MOTOR_TYPE=  0;
int HW_LIGHT_TYPE=  0;
int HW_SPK_VOLUME=  2;
int GET_IP_FLAG  =  0;
int AUTO_CONNECT =  0;
int REMOVE_DEVICE=  0;
int32_t get_build_num(void)
{
    uint8_t year = 0, month = 0, day = 0, hour = 0, min = 0;
    const char *pMonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    const char *pDate    = __DATE__;
    const char *pTime    = __TIME__;
    uint8_t i;
    for (i=0; i<12; i++) {
        if (memcmp(pDate, pMonth[i], 3) == 0) {
            month = i + 1;
            break;
        }
    }
    year = (uint8_t)atoi(pDate + 9);
    day  = (uint8_t)atoi(pDate + 4);
    hour = (uint8_t)atoi(pTime + 0);
    min  = (uint8_t)atoi(pTime + 3);
    return year * 100000000L + month * 1000000L + day * 10000L + hour * 100L + min * 1L;
}

static void parse_hw_info_str(char *infostr, char *key, int *val)
{
    char *str = strstr(infostr, key);
    if (str && val) *val = atoi(str + strlen(key));
}

void apkapi_init(void)
{
    char cmd[128];
    file_read("/tmp/hwinfostr", HW_INFO_STR, sizeof(HW_INFO_STR));
    if (strstr(HW_INFO_STR, "hwinfostr=")) strncpy(HW_INFO_STR, HW_INFO_STR + 10, sizeof(HW_INFO_STR));
    parse_hw_info_str(HW_INFO_STR, "R_", &GPIO_LED_R   );
    parse_hw_info_str(HW_INFO_STR, "G_", &GPIO_LED_G   );
    parse_hw_info_str(HW_INFO_STR, "B_", &GPIO_LED_B   );
    parse_hw_info_str(HW_INFO_STR, "U_", &GPIO_LED_U   );
    parse_hw_info_str(HW_INFO_STR, "WI", &GPIO_WIFI    );
    parse_hw_info_str(HW_INFO_STR, "SP", &GPIO_SPK_PWR );
    parse_hw_info_str(HW_INFO_STR, "IR", &GPIO_IR_LED  );
    parse_hw_info_str(HW_INFO_STR, "CA", &GPIO_IR_CUT0 );
    parse_hw_info_str(HW_INFO_STR, "CB", &GPIO_IR_CUT1 );
    parse_hw_info_str(HW_INFO_STR, "LR", &LED_REVERSED );
    parse_hw_info_str(HW_INFO_STR, "FL", &CAM_HV_FLIP  );
    parse_hw_info_str(HW_INFO_STR, "IQ", &CAM_IQ_FILE  );
    parse_hw_info_str(HW_INFO_STR, "M_", &HW_MOTOR_TYPE);
    parse_hw_info_str(HW_INFO_STR, "L_", &HW_LIGHT_TYPE);
    parse_hw_info_str(HW_INFO_STR, "V_", &HW_SPK_VOLUME);
    snprintf(cmd, sizeof(cmd), "echo %d > /tmp/gpio_upd_led", GPIO_LED_U ); system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d > /tmp/gpio_wifi"   , GPIO_WIFI  ); system(cmd);
}

int file_exist(char *file)
{
    int fd = open(file, O_RDONLY);
    close(fd);
    return fd < 0 ? 0 : 1;
}

void file_write(char *file, char *data)
{
    int fd = open(file, O_WRONLY);
    if (fd < 0) return;
    write(fd, data, strlen(data));
    close(fd);
}

int file_read(char *file, char *data, int len)
{
    int ret;
    int fd = open(file, O_RDONLY);
    if (fd < 0) return -1;
    ret = read(fd, data, len-1);
    data[ret < len ? ret : len - 1] = '\0';
    close(fd);
    return ret;
}

int get_gpio(int gpio)
{
    char strval [8  ];
    char strfile[256];
    int  val;

    if (gpio < 0) return -1;
    snprintf(strval, sizeof(strval), "%d", gpio);
    file_write("/sys/class/gpio/export", strval);

    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/direction", gpio);
    file_write(strfile, "in");

    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/value", gpio);
    file_read(strfile, strval, sizeof(strval));
    val = atoi(strval);

    snprintf(strval, sizeof(strval), "%d", gpio);
    file_write("/sys/class/gpio/unexport", strval);
    return val;
}

void set_gpio(int gpio, int val)
{
    char strval [8  ];
    char strfile[256];

    if (gpio < 0) return;
    snprintf(strval , sizeof(strval ), "%d", gpio);
    file_write("/sys/class/gpio/export", strval);

    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/direction", gpio);
    file_write(strfile, "out");

    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/value", gpio);
    snprintf(strval , sizeof(strval ), "%d", val);
    file_write(strfile, strval);

    snprintf(strval , sizeof(strval ), "%d", gpio);
    file_write("/sys/class/gpio/unexport", strval);
}

void set_led(int gpio, int val)
{
    set_gpio(gpio, !LED_REVERSED ? val : !val);
}

int get_led(int gpio)
{
    char strval [8  ];
    char strfile[256];
    int  val;

    if (gpio < 0) return -1;
    snprintf(strval, sizeof(strval), "%d", gpio);
    file_write("/sys/class/gpio/export", strval);

    snprintf(strfile, sizeof(strfile), "/sys/class/gpio/gpio%d/value", gpio);
    file_read(strfile, strval, sizeof(strval));
    val = atoi(strval);

    snprintf(strval, sizeof(strval), "%d", gpio);
    file_write("/sys/class/gpio/unexport", strval);
    return !LED_REVERSED ? val : !val;
}

void set_ircut(int en)
{
    printf("en = %d\n",en);
    if (GPIO_IR_CUT0 != -1 && GPIO_IR_CUT1 != -1) {
        set_gpio(GPIO_IR_CUT0, !en);
        set_gpio(GPIO_IR_CUT1,  en); usleep(300*1000);
        get_gpio(GPIO_IR_CUT0);
        get_gpio(GPIO_IR_CUT1);
    };
}

int get_cpu_uid(char *uid, int size)
{
    MSYS_UDID_INFO info = {};
    char *pbyte = (char*)&info.udid;
    int   fd    = -1;
    int   ret   = -1;

    fd = open("/dev/msys", O_RDWR, 606);
    if (fd < 0) {
        return -1;
    }

    info.VerChk_Version = 0x4D530000 | 0x0100;
    info.VerChk_Size    = sizeof(info);
    if (0 == ioctl(fd, IOCTL_MSYS_GET_UDID, &info)) {
        snprintf(uid, size, "%02x%02x%02x%02x%02x%02x%02x%02x",
            pbyte[7], pbyte[6], pbyte[5], pbyte[4],
            pbyte[3], pbyte[2], pbyte[1], pbyte[0]);
    } else {
        goto done;
    }
    ret = 0;

done:
    close(fd);
    return ret;
}

typedef struct {
    char signature[9];
    char mac_eth [18];
    char mac_wlan[18];
    char dev_sid [33];
    char dev_uid [33];
    char dev_sn  [11];
} DEVIDDATA;

static int mtd_read_ids_data(DEVIDDATA *data)
{
    #define MTD_DEV_IDS "/dev/mtd5"
    struct mtd_dev_info info;
    libmtd_t            mtd ;
    int      fd, i, ret = -1;

    mtd = libmtd_open();
    if (!mtd) return -1;

    mtd_get_dev_info(mtd, MTD_DEV_IDS, &info);
    fd = open(MTD_DEV_IDS, O_RDONLY);
    if (fd < 0) goto done;
    for (i=0; i<info.eb_cnt; i++) {
        if (!mtd_is_bad(&info, fd, i)) {
            ret = mtd_read(&info, fd, i, 0, data, sizeof(DEVIDDATA));
            break;
        }
    }
    close(fd);

done:
    if (mtd) libmtd_close(mtd);
    return ret;
}

static int mtd_write_ids_data(DEVIDDATA *data)
{
    #define MTD_DEV_IDS "/dev/mtd5"
    struct mtd_dev_info info;
    libmtd_t            mtd ;
    int      fd, i, ret = -1;

    mtd = libmtd_open();
    if (!mtd) return -1;

    mtd_get_dev_info(mtd, MTD_DEV_IDS, &info);
    fd = open(MTD_DEV_IDS, O_WRONLY);
    if (fd < 0) goto done;
    for (i=0; i<info.eb_cnt; i++) {
        if (!mtd_is_bad(&info, fd, i)) {
            mtd_erase(mtd, &info, fd, i);
            ret = mtd_write(mtd, &info, fd, i, 0, data, sizeof(DEVIDDATA), NULL, 0, 0);
            break;
        }
    }
    close(fd);

done:
    if (mtd) libmtd_close(mtd);
    return ret;
}

int get_dev_sid(char *sid, int size)
{
    DEVIDDATA data;
    int       i;
    if (mtd_read_ids_data(&data) < 0) return -1;
    if (memcmp(data.signature, "APKIPCAM", 8) == 0) {
        memcpy(sid, data.dev_sid, size < sizeof(data.dev_sid) ? size : sizeof(data.dev_sid));
        sid[size < sizeof(data.dev_sid) ? size-1 : sizeof(data.dev_sid)] = '\0';
    } else {
        sid[0] = '\0';
    }
    for (i=0; i<size; i++) {
        if (sid[i] == 0xff) {
            sid[i] = 0;
            break;
        }
    }
    return 0;
}

int get_dev_uid(char *uid, int size)
{
    DEVIDDATA data;
    int       i;
    if (mtd_read_ids_data(&data) < 0) return -1;
    if (memcmp(data.signature, "APKIPCAM", 8) == 0) {
        memcpy(uid, data.dev_uid, size < sizeof(data.dev_uid) ? size : sizeof(data.dev_uid));
        uid[size < sizeof(data.dev_uid) ? size-1 : sizeof(data.dev_uid)] = '\0';
    } else {
        uid[0] = '\0';
    }
    for (i=0; i<size; i++) {
        if (uid[i] == 0xff) {
            uid[i] = 0;
            break;
        }
    }
    return 0;
}

//zyh add 
int get_dev_sn (char *sn, int size)
{
    DEVIDDATA data;
    int       i;
    if (mtd_read_ids_data(&data) < 0) return -1;
    if (memcmp(data.signature, "APKIPCAM", 8) == 0) {
        memcpy(sn, data.dev_sn, size < sizeof(data.dev_sn) ? size : sizeof(data.dev_sn));
        sn[size < sizeof(data.dev_sn) ? size-1 : sizeof(data.dev_sn)] = '\0';
    } else {
        sn[0] = '\0';
    }
    for (i=0; i<size; i++) {
        if (sn[i] == 0xff) {
            sn[i] = 0;
            break;
        }
    }
    return 0;
}

int set_dev_ids(char *mac_eth, char *mac_wlan, char *dev_sid, char *dev_uid, char *dev_sn)
{
    DEVIDDATA newdata = {};
    DEVIDDATA olddata;
    if (mtd_read_ids_data(&olddata) < 0) return -1;
    strncpy(newdata.signature, "APKIPCAM", sizeof(newdata.signature));
    strncpy(newdata.mac_eth , mac_eth ? mac_eth : olddata.mac_eth , sizeof(newdata.mac_eth ));
    strncpy(newdata.mac_wlan, mac_wlan? mac_wlan: olddata.mac_wlan, sizeof(newdata.mac_wlan));
    strncpy(newdata.dev_sid , dev_sid ? dev_sid : olddata.dev_sid , sizeof(newdata.dev_sid ));
    strncpy(newdata.dev_uid , dev_uid ? dev_uid : olddata.dev_uid , sizeof(newdata.dev_uid ));
    strncpy(newdata.dev_sn  , dev_sn  ? dev_sn  : olddata.dev_sn  , sizeof(newdata.dev_sn  ));
    if (memcmp(&olddata, &newdata, sizeof(DEVIDDATA)) != 0) {
        return mtd_write_ids_data(&newdata);
    }
    return 0;
}

//zyh add
int get_dev_mac(char *str)
{
    char buf[128];
    int i = 19;
    int j = 0;
    system("rtwpriv wlan0 efuse_get mac > /tmp/wlan_mac");
    memset(buf, 0, sizeof(buf));
    file_read("/tmp/wlan_mac", buf, sizeof(buf)); 
    while(buf[i] != '\0')
    {
        str[j++] = buf[i++];
    }
    printf("j = %d\n",j);
	for(i=j=0;str[i]!='\0';i++)
    {
		if(str[i] != ':')
        {
			str[j++]=str[i];
		}
	}
	str[j]='\0';
    printf("j = %d\n",j);
    printf("wlan_mac = %s\n",str);
    return 0; 
}

int get_dev_ip(char *dev, char *str, int len)
{
    int    sock;
    struct ifreq  ifr = {};
    struct in_addr ip = {};
    strncpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;
    ioctl(sock, SIOCGIFADDR, &ifr);
    close(sock);

    ip = ((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr;
    if (str) strncpy(str , inet_ntoa(ip), len);
    return ip.s_addr ? 0 : -1;
}

void get_dev_gateway_mask(char *dev, char *gw, char *mask)
{
    char     buf[1024];
    char    *str;
    uint32_t val, tmp;
    strcpy(gw  , "0.0.0.0");
    strcpy(mask, "0.0.0.0");
    file_read("/proc/net/route", buf, sizeof(buf));
    str = strstr(buf, dev);
    if (str) {
        str += strlen(dev);
        sscanf (str, "%x %x", &tmp, &val);
        sprintf(gw , "%d.%d.%d.%d", (val >> 0) & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
        str  = strstr(str, dev);
        if (str) {
            str += strlen(dev);
            sscanf (str , "%x %x %x %x %x %x %x", &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &val);
            sprintf(mask, "%d.%d.%d.%d", (val >> 0) & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
        }
    }
}

int get_wifi_signal(void)
{
    char  buf[256];
    char *str;
    float tmp, val;
    file_read("/proc/net/wireless", buf, sizeof(buf));
    str = strstr(buf, "wlan0:");
    if (!str) return -256;
    str+= 6;
    sscanf(str, "%f %f %f", &tmp, &tmp, &val);
    return (int)val;
}

int get_netdev_status(char *dev)
{
    char buf[512];
    file_read("/proc/net/dev", buf, sizeof(buf));
    return !!strstr(buf, dev);
}

int get_sdcard_status(uint32_t *status, int64_t *total, int64_t *free)
{
    uint32_t s = 0;
    int64_t  t = 0, f = 0;
    struct statfs stat;

    if (status) s = *status;
    if (!get_gpio(GPIO_SD_DET)) {
        s |= SDSTATUS_INSERTED;
        if (file_exist("/dev/mmcblk0") || file_exist("/dev/mmcblk0p1")) {
            s |= SDSTATUS_DETECTED;
            if (0 == statfs(SDCARD_MOUNT_PATH, &stat)) {
                t = (int64_t)stat.f_bsize * stat.f_blocks;
                f = (int64_t)stat.f_bsize * stat.f_bfree ;
                s |= SDSTATUS_MOUNTED;
            }
        }
    }

    if (status) *status = s;
    if (total ) *total  = t;
    if (free  ) *free   = f;
    return !!t;
}

void get_sdcard_status_from_shm(uint32_t *s, int64_t *t, int64_t *f)
{
    SDSTATUS *sdstatus = NULL;
    int       shmid    = shmget((key_t)SDSTATUS_SHMID, sizeof(SDSTATUS), 0666|IPC_CREAT);
    if (shmid != -1) {
        sdstatus = (SDSTATUS*)shmat(shmid, NULL, 0);
        if (sdstatus == (SDSTATUS*)-1) {
            sdstatus = NULL;
        }
    }
    if (s) *s = sdstatus ? sdstatus->status   : 0;
    if (t) *t =(sdstatus && (sdstatus->status & SDSTATUS_MOUNTED)) ? sdstatus->total     : 0;
    if (f) *f =(sdstatus && (sdstatus->status & SDSTATUS_MOUNTED)) ? sdstatus->available : 0;
    if (sdstatus) shmdt(sdstatus);
}

uint32_t get_tick_count(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

uint64_t get_time_stamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

void http_string_decode(char *str)
{
    int   len = strlen(str);
    char *tmp = malloc(len+1);
    char *dst, *src, *end;
    if (tmp) {
        strcpy(tmp, str);
        dst = str;
        src = tmp;
        end = tmp + len;
        while (src < end) {
            if (*src != '%') {
                *dst++ = *src++;
            } else {
                char hex[3];
                int  val;
                memcpy(hex, ++src, 2); hex[2] = '\0';
                sscanf(hex, "%x", &val);
                *dst++ = val;
                 src  += 2;
            }
        }
        *dst = '\0';
        free(tmp);
    }
}

char* parse_params(const char *str, const char *key, char *val, int len)
{
    char *p = (char*)strstr(str, key);
    int   i;

    *val = '\0';
    if (!p) return NULL;
    p += strlen(key);
    if (*p == '\0') return NULL;

    while (*p) {
        if (*p != ':' && *p != '"' && *p != '=' && *p != ' ') break;
        else p++;
    }

    for (i=0; i<len; i++) {
        if (*p == '\\') {
            p++;
        } else if (*p == '"' || *p == ',' || *p == '&' || *p == '\n' || *p == '\0') {
            break;
        }
        val[i] = *p++;
    }
    val[i] = val[len-1] = '\0';
    return val;
}

int check_proc_running(char *name)
{
    char  path[256];
    FILE *fp = NULL;
    int   pid= 0;

    snprintf(path, sizeof(path), "/tmp/%s_pid", name);
    if (!file_exist(path)) {
        fp = fopen(path, "wb");
        if (fp) {
            fprintf(fp, "%d", getpid());
            fclose(fp);
        }
        return -1;
    } else {
        fp = fopen(path, "rb");
        if (fp) {
            fscanf(fp, "%d", &pid);
            fclose(fp);
        }
        if (pid && kill(pid, 0) == 0) {
            return pid;
        }
    }
    return -2;
}

void exit_proc_running(char *name)
{
    char path[256];
    snprintf(path, sizeof(path), "/tmp/%s_pid", name);
    unlink(path);
}

void str2lower(char *str)
{
    while (*str) {
        *str = tolower(*str);
         str++;
    }
}

int send_msg_to_ipcam(char *msg, char *params)
{
    #define FACTORY_TEST_PORT 8313
    struct    sockaddr_in servaddr = {};
    socklen_t socklen  = sizeof(struct sockaddr_in);
    int       sock     = -1;
    char      txbuf[512];
    char      rxbuf[128];
    int       opt, ret, id;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) 
        return -1;
    opt = 10; setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO , (char*)&opt, sizeof(int));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port        = htons(FACTORY_TEST_PORT);
    id = get_tick_count() % 10000;
    snprintf(txbuf, sizeof(txbuf), "rpc! %d %s %s", id, msg, params);
    ret = sendto  (sock, txbuf, strlen(txbuf) + 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    ret = recvfrom(sock, rxbuf, sizeof(rxbuf)    , 0, (struct sockaddr*)&servaddr, &socklen);
    close(sock);
    return ret > 0 && strstr(rxbuf, "rpc. ") == rxbuf && atoi(rxbuf + 5) == id ? 0 : -1;
}

//zyh add
int set_wlan_mac(char *mac)
{
    char cmd[256];
    printf("mac = %s\n\n",mac);
    snprintf(cmd, sizeof(cmd), "rtwpriv wlan0 efuse_set mac,%s",mac);
    printf("cmd = %s\n",cmd);
    system(cmd);
    sleep(2);
    //set_wlan_map();
}

int set_wlan_map()
{
    system("rtwpriv wlan0 mp_start");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x00,298100CC0B000000000C044C100C0000");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x010,2929282828282929291B1B02FFFFFFFF");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x0B0,FFFFFFFFFFFFFFFF20202500000000FF");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x0C0,FF11001000FF00FF0000FFFFFFFFFFFF");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x0D0,DA0B79F142664000E04CF17900090352");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x0E0,65616C74656B09033830322E31316E00");
    sleep(2);
    system("rtwpriv wlan0 efuse_set wlwfake,0x130,C1AEFFFFFFFFFFFFFFFF0011FFFFFFFF");
    sleep(2);  
    system("rtwpriv wlan0 efuse_set wlfk2map");
    sleep(2);  
}

int get_wlan_map_and_compare(void)
{
    char buf[128];
    char cmd[128];
    int  val = 0;
    int    i = 0;
    int    j = 0;
    int  line[7] = {0x00, 0x10, 0xB0, 0xC0, 0xD0, 0xE0, 0x130};
    char *str[7] = {"wlan0    efuse_get:0x29 0x81 0x00 0xCC 0x0B 0x00 0x00 0x00 0x00 0x0C 0x04 0x4C 0x10 0x0C 0x00 0x00",
                    "wlan0    efuse_get:0x29 0x29 0x28 0x28 0x28 0x28 0x29 0x29 0x29 0x1B 0x1B 0x02 0xFF 0xFF 0xFF 0xFF",
                    "wlan0    efuse_get:0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0x20 0x20 0x25 0x00 0x00 0x00 0x00 0xFF",
                    "wlan0    efuse_get:0xFF 0x11 0x00 0x10 0x00 0xFF 0x00 0xFF 0x00 0x00 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF",
                    "wlan0    efuse_get:0xDA 0x0B 0x79 0xF1 0x42 0x66 0x40 0x00 0xE0 0x4C 0x87 0x12 0x34 0x09 0x03 0x52",
                    "wlan0    efuse_get:0x65 0x61 0x6C 0x74 0x65 0x6B 0x09 0x03 0x38 0x30 0x32 0x2E 0x31 0x31 0x6E 0x00",
                    "wlan0    efuse_get:0xC1 0xAE 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0x00 0x11 0xFF 0xFF 0xFF 0xFF"};
    for (i = 0; i < 7; i++)
    {
        memset(buf, 0, sizeof(buf));
        snprintf(cmd, sizeof(cmd), "rtwpriv wlan0 efuse_get rmap,%#X,16 > tmp/wlan_map", line[i]);
        system(cmd);
        file_read("/tmp/wlan_map", buf, sizeof(buf));
        system("rm tmp/wlan_map");
        int result = strncmp(buf, str[i], 98);
        if (result != 0) j++;
    }
    return j;
}