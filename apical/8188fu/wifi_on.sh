#!/bin/sh

echo 14 > /tmp/gpio_wifi
WIFI_GPIO=`cat /tmp/gpio_wifi`

if [ ! -d /sys/class/gpio/gpio$WIFI_GPIO ]; then
    echo $WIFI_GPIO > /sys/class/gpio/export
fi
echo out > /sys/class/gpio/gpio$WIFI_GPIO/direction
echo 0 > /sys/class/gpio/gpio$WIFI_GPIO/value
echo $WIFI_GPIO > /sys/class/gpio/unexport

insmod /lib/modules/4.9.84/usb-common.ko
insmod /lib/modules/4.9.84/usbcore.ko
insmod /lib/modules/4.9.84/ehci-hcd.ko force_host=1
insmod /lib/modules/4.9.84/cfg80211.ko
insmod /lib/modules/4.9.84/ssw101b_wifi_usb.ko
sleep 2
ifconfig wlan0 up