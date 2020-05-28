#!/bin/bash

set -e

./fw_setenv bootcmd "sf probe 0;sf read 0x21000000 50000 200000;bootm 0x21000000"
./fw_setenv bootargs "console=ttyS0,115200 root=/dev/mtdblock2 rootfstype=squashfs ro init=/linuxrc LX_MEM=0x3fc6000 mma_heap=mma_heap_name0,miu=0,sz=0x2000000"
./fw_setenv sf_kernel_size 180000
./fw_setenv sf_kernel_start 50000
./fw_setenv filesize 0x7f0000
./fw_setenv stderr serial
./fw_setenv stdin  serial
./fw_setenv stdout serial
./fw_setenv autoestart 0
./fw_setenv hwinfostr "$1"
