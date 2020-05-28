#!/bin/sh

    echo "sd card insert!" > /dev/console

    if [ -e "/dev/mmcblk0p1"  ]; then
        mount -t vfat /dev/mmcblk0p1 /tmp/sdcard
    elif [ -e "/dev/mmcblk0"  ]; then
        mount -t vfat /dev/mmcblk0 /tmp/sdcard
    else
        echo "mount sd card failed!" > /dev/console
    fi
    ;;