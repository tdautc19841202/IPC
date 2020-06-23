#!/bin/bash

set -e

IMAGES_DIR=$PWD/image/output/images

if [ -f "one.bin" ]; then
echo "** delete one.bin **"
rm -f one.bin
fi
echo "** combine_one  ALL **"
echo "** spi start address = 0x0000  **"
echo "Must follow MXP layout"
dd if=$IMAGES_DIR/boot/IPL.bin of=./one.bin bs=1024 count=64
dd if=$IMAGES_DIR/boot/IPL_CUST.bin of=./one.bin bs=1024 count=64 seek=64
dd if=$IMAGES_DIR/boot/MXP_SF.bin of=./one.bin bs=1024 count=64 seek=128
dd if=$IMAGES_DIR/boot/u-boot.xz.img.bin of=./one.bin bs=1024 count=128 seek=192
dd if=$IMAGES_DIR/kernel of=./one.bin bs=1024 count=1536 seek=320
dd if=$IMAGES_DIR/rootfs.sqfs of=./one.bin bs=1024 count=2624 seek=1856
dd if=$IMAGES_DIR/miservice.sqfs of=./one.bin bs=1024 count=1664 seek=4480
dd if=$IMAGES_DIR/customer.jffs2 of=./one.bin bs=1024 count=1984 seek=6144