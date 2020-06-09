#!/bin/sh

WIFI_GPIO=`cat /tmp/gpio_wifi`

if [ ! -d /sys/class/gpio/gpio$WIFI_GPIO ]; then
    echo $WIFI_GPIO > /sys/class/gpio/export
fi
echo out > /sys/class/gpio/gpio$WIFI_GPIO/direction
echo 1 > /sys/class/gpio/gpio$WIFI_GPIO/value
echo $WIFI_GPIO > /sys/class/gpio/unexport

insmod /lib/modules/4.9.84/usb-common.ko
insmod /lib/modules/4.9.84/usbcore.ko
insmod /lib/modules/4.9.84/ehci-hcd.ko force_host=1
insmod /lib/modules/4.9.84/cfg80211.ko
insmod /lib/modules/4.9.84/8188fu.ko

#dd if=/dev/mtdblock5 of=/tmp/apkipcam bs=1 count=8
#dd if=/dev/mtdblock5 of=/tmp/wlan_mac bs=1 count=17 skip=27
#APK_TAG=`cat /tmp/apkipcam`
#WLAN_MAC=`cat /tmp/wlan_mac`
#rm /tmp/apkipcam
#rm /tmp/wlan_mac
if [ "$APK_TAG"x = "APKIPCAM"x -a "$WLAN_MAC"x != "00:00:00:00:00:00"x ]; then
	ifconfig wlan0 hw ether $WLAN_MAC
else
	num1=`expr $RANDOM % 100`
	num2=`expr $RANDOM % 100`
	num3=`expr $RANDOM % 100`
	ifconfig wlan0 hw ether 00:e0:4c:$num1:$num2:$num3
fi