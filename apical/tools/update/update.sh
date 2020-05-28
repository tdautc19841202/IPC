#!/bin/sh

echo "start update ..."
chmod +x /tmp/update/*.sh
# /tmp/update/runled.sh &

#++ kill ipcam & thttpd & umount customer
killall watchdog.sh ipcam thttpd

if [ ! -d "/tmp/update/app" ]; then
umount /customer
while [ $? -ne 0 ]; do
    echo "try to umount /customer ..."
    sleep 1
    umount /customer
done
fi
#-- kill ipcam & thttpd & umount customer

rm -rf /customer/bin/ipcam
RESULT_1=0
RESULT_2=0
RESULT_3=0
RESULT_4=0
RESULT_5=0
RESULT_6=0

# write update to customer partition
if [ -d "/tmp/update/app" ]; then
    #cp -rp /tmp/update/app/bin/ipcam /customer/bin/
    dd if=/tmp/update/app/bin/ipcam of=/customer/bin/ipcam bs=1024 count=5800
    chmod a+x /customer/bin/ipcam
    RESULT_1=$?
fi
if [ -f "/tmp/update/customer.jffs2" ]; then
    /tmp/update/flash_erase /dev/mtd/mtd4 0 0
    RESULT_1=$?
    /tmp/update/nandwrite -p /dev/mtd/mtd4 /tmp/update/customer.jffs2
    RESULT_2=$?
fi

if [ -f "/tmp/update/nvrservice.sqfs" ]; then
    /tmp/update/flash_erase /dev/mtd/mtd3 0 0
    RESULT_3=$?
    /tmp/update/nandwrite -p /dev/mtd/mtd3 /tmp/update/nvrservice.sqfs
    RESULT_4=$?
fi

if [ -f "/tmp/update/rootfs.sqfs" ]; then
    /tmp/update/flash_erase /dev/mtd/mtd2 0 0
    RESULT_5=$?
    /tmp/update/nandwrite -p /dev/mtd2 /tmp/update/rootfs.sqfs
    RESULT_6=$?
fi
if [ $RESULT_1 -eq 0 -a $RESULT_2 -eq 0 -a $RESULT_3 -eq 0 -a $RESULT_4 -eq 0 -a $RESULT_5 -eq 0 -a $RESULT_6 -eq 0 ]; then
    fw_setenv bootcmd  "sf probe 0;sf read 0x21000000 50000 200000;bootm 0x21000000"
    fw_setenv bootargs "console=ttyS0,115200 root=/dev/mtdblock2 rootfstype=squashfs ro init=/linuxrc LX_MEM=0x3fc6000 mma_heap=mma_heap_name0,miu=0,sz=0x2000000"
    echo "update done !"

    # stop led run
    # killall runled.sh

    # turn on led
    # /tmp/update/setled.sh 1

    # rm -rf /customer/data/ipcam/ipcam.ini
    sync

    if [ ! -f "/tmp/update/sdupdate" ]; then
        sleep 1 && reboot -f
    fi
else
    echo "update failed !"

    # stop led run
    # killall runled.sh

    # turn on red led
    # /tmp/update/setled.sh 0
fi

