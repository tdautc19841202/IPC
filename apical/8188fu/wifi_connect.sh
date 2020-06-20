#!/bin/sh

WIFI_SSID=$1
WIFI_PSK=$2
if [ "$WIFI_SSID"x = ""x ]; then
    WIFI_SSID="hp"
    WIFI_PSK="zyh1567890"
fi

if [ "$1"x = ""x -a "$2"x = ""x -a -f /customer/data/cfg/wpa_supplicant.conf ]; then
echo "use last wpa_supplicant.conf file ..."
else
echo WIFI_SSID: $WIFI_SSID
echo WIFI_PSK : $WIFI_PSK
if [ "$WIFI_PSK"x = ""x ]; then
echo "ctrl_interface=/customer/data/cfg/wpa_supplicant
network={
    ssid=\"$WIFI_SSID\"
    key_mgmt=NONE
    scan_ssid=1
}
" > /customer/data/cfg/wpa_supplicant.conf
else
echo "ctrl_interface=/customer/data/cfg/wpa_supplicant
network={
    ssid=\"$WIFI_SSID\"
    psk=\"$WIFI_PSK\"
    scan_ssid=1
}
" > /customer/data/cfg/wpa_supplicant.conf
fi
fi


kill `ps | awk '$5=="udhcpc" && $7=="wlan0" {printf $1}'`
killall wpa_cli wpa_supplicant

for i in { 1..3 }; do
    echo "wait wifi driver loaded ..."
    WLANINFO=`cat /proc/net/dev | grep wlan0`;
    if [ "$WLANINFO"x != ""x ]; then
        break;
    fi
    sleep 1
done

wpa_supplicant -i wlan0 -Dnl80211 -c /customer/data/cfg/wpa_supplicant.conf -B

for i in { 1..3 }; do
    echo "wait wpa_supplicant started ..."
    if [ -e "/customer/data/cfg/wpa_supplicant/wlan0" ]; then
        break;
    fi
    sleep 1
done

sleep 3
udhcpc -i wlan0 -T 10 &
wifi_check.sh &
