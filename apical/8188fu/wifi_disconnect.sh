#!/bin/sh

kill `ps | awk '$5=="udhcpc" && $7=="wlan0" {printf $1}'`
killall wpa_cli wpa_supplicant udhcpd hostapd
ifconfig wlan0 down