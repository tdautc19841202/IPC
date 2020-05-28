#!/bin/sh
ISERVER_PATH=/customer/mi_demo/iserver/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ISERVER_PATH/dnslib
ifconfig eth0 up
ifconfig eth0 hw ether 00:88:32:ED:E1:A2
udhcpc -o eth0 -s /etc/init.d/udhcpc.script

export LD_LIBRARY_PATH=$ISERVER_PATH/lib:$ISERVER_PATH/thirdlib
export ISOTA_CUS_CFG_PATH=$ISERVER_PATH/OTA/config/is_cus_cfg.xml
echo $LD_LIBRARY_PATH
cd $ISERVER_PATH
./prog_iserver $@
#valgrind --tool=memcheck --leak-check=full --show-reachable=yes --trace-children=yes    ./fw_demo $@
cp /customer/mi_demo/ota/prog_ota /tmp
cp /customer/mi_demo/iserver/OTA/SStarOta.bin /tmp
cd /
umount /customer
umount /config
umount /appconfigs
/tmp/prog_ota /tmp/SStarOta.bin
reboot