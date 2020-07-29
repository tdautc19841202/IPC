#!/bin/sh

WIFI_GPIO_A=`cat /tmp/gpio_wifi`

#++ wifi stop
kill `ps | awk '$5=="udhcpc" && $7=="wlan0" {printf $1}'`
killall wpa_cli wpa_supplicant

rmmod -f ssw101b_wifi_usb
rmmod -f cfg80211
rmmod -f ehci_hcd
rmmod -f usbcore
rmmod -f usb_common

if [ ! -d /sys/class/gpio/gpio$WIFI_GPIO_A ]; then
    echo $WIFI_GPIO_A > /sys/class/gpio/export
fi
echo out > /sys/class/gpio/gpio$WIFI_GPIO_A/direction
echo 0 > /sys/class/gpio/gpio$WIFI_GPIO_A/value
echo $WIFI_GPIO_A > /sys/class/gpio/unexport

#-- wifi stop

#++ wifi start
if [ ! -d /sys/class/gpio/gpio$WIFI_GPIO_A ]; then
    echo $WIFI_GPIO_A > /sys/class/gpio/export
fi
echo out > /sys/class/gpio/gpio$WIFI_GPIO_A/direction
echo 1 > /sys/class/gpio/gpio$WIFI_GPIO_A/value
echo $WIFI_GPIO_A > /sys/class/gpio/unexport

insmod /lib/modules/4.9.84/usb-common.ko
insmod /lib/modules/4.9.84/usbcore.ko
insmod /lib/modules/4.9.84/ehci-hcd.ko force_host=1
insmod /lib/modules/4.9.84/cfg80211.ko
insmod /lib/modules/4.9.84/ssw101b_wifi_usb.ko

for i in { 1..3 }; do
    echo "wait wifi driver loaded ..."
    WLANINFO=`cat /proc/net/dev | grep wlan0`;
    if [ "$WLANINFO"x != ""x ]; then
        break;
    fi
    sleep 1
done
#-- wifi start


#++ wifi mp test start
ifconfig wlan0 up
rtwpriv wlan0 mp_start
rtwpriv wlan0 mp_channel 6
rtwpriv wlan0 mp_bandwidth 40M=0,shortGI=0
rtwpriv wlan0 mp_ant_tx a
rtwpriv wlan0 mp_txpower patha=45,pathb=45
rtwpriv wlan0 mp_rate 108
rtwpriv wlan0 mp_ctx background,pkt
#-- wifi mp test start
