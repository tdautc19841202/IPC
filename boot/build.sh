#!/bin/sh

set -e

PROJ_UBOOT_RELDIR=$PWD/../project/board/i6b0/boot/nor/uboot

case "$1" in
"")
    if [ ! -e .config ]; then
        make infinity6b0_defconfig
    fi
    make -j8
	mkdir -p $PROJ_UBOOT_RELDIR
    cp $PWD/u-boot.xz.img.bin $PROJ_UBOOT_RELDIR
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    ;;
esac
