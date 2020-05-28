#!/bin/sh

ETHNET_EN=`awk '$1=="ethnet_en" {printf $3}' /customer/data/ipcam/ipcam.ini`

if [ "$ETHNET_EN"x = "1"x ]; then
    ifconfig eth0 up

#   echo "APKIPCAM 08:00:20:64:72:09 00:00:00:00:00:00 IZSPIOI4D7CJZMNBZEBD" > /dev/block/mtdblock5
#   dd if=/dev/block/mtdblock5 of=/tmp/wlan_mac bs=1 count=17 skip=27
#   dd if=/dev/block/mtdblock5 of=/tmp/serialno bs=1 count=20 skip=45

    dd if=/dev/mtdblock5 of=/tmp/apkipcam bs=1 count=8
    dd if=/dev/mtdblock5 of=/tmp/eth_mac  bs=1 count=17 skip=9

    APK_TAG=`cat /tmp/apkipcam`
    ETH_MAC=`cat /tmp/eth_mac`
    rm /tmp/apkipcam
    rm /tmp/eth_mac

    if [ "$APK_TAG"x = "APKIPCAM"x -a "$ETH_MAC"x != "00:00:00:00:00:00"x ]; then
        ifconfig eth0 hw ether $ETH_MAC
    else
        num1=`expr $RANDOM % 100`
        num2=`expr $RANDOM % 100`
        num3=`expr $RANDOM % 100`
        ifconfig eth0 hw ether 08:00:20:$num1:$num2:$num3
    fi

    udhcpc -i eth0 &
fi

fw_printenv hwinfostr > /tmp/hwinfostr
mkdir -p /customer/data/ipcam
watchdog.sh &
