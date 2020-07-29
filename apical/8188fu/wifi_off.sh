#!/bin/sh

WIFI_GPIO=`cat /tmp/gpio_wifi`


rmmod -f ssw101b_wifi_usb
rmmod -f cfg80211
rmmod -f ehci_hcd
rmmod -f usbcore
rmmod -f usb_common

if [ ! -d /sys/class/gpio/gpio$WIFI_GPIO ]; then
    echo $WIFI_GPIO > /sys/class/gpio/export
fi
echo out > /sys/class/gpio/gpio$WIFI_GPIO/direction
echo 0 > /sys/class/gpio/gpio$WIFI_GPIO/value
echo $WIFI_GPIO > /sys/class/gpio/unexport
