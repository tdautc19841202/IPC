#!/bin/sh

NET_STATUS=down

while [ -f "/customer/data/cfg/wpa_supplicant.conf" ]
do
    if [ "up" = "$(cat /sys/class/net/wlan0/operstate)" -a "$NET_STATUS" = "down" ];then
      echo "WPA supplicant: connection established";
      ipcam mp3 /customer/res/connected.mp3;
      NET_STATUS=up
      sleep 2
    fi

    if [ "down" = "$(cat /sys/class/net/wlan0/operstate)" -a "$NET_STATUS" = "up" ];then
      echo "WPA supplicant: wifi connection was lost";
      ipcam mp3 /customer/res/disconnect.mp3;
      kill `ps | awk '$5=="udhcpc" && $7=="wlan0" {printf $1}'`
      ifconfig wlan0 0.0.0.0 && udhcpc -i wlan0 -T 10&
      NET_STATUS=down
      sleep 2
    fi
done
