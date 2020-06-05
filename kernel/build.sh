#!/bin/sh

set -e

PROJ_KERNEL_RELDIR=$PWD/../project/release/ipc/i6b0/009A/uclibc/4.9.4/bin/kernel/nor

case "$1" in
"")
    if [ ! -e .config ]; then
        make infinity6b0_ssc009a_s01a_defconfig
    fi
    make -j8
	mkdir -p $PROJ_KERNEL_RELDIR
    #cp $PWD/arch/arm/boot/Image-fpga $PROJ_KERNEL_RELDIR
    cp $PWD/arch/arm/boot/uImage.mz  $PROJ_KERNEL_RELDIR
    cp $PWD/arch/arm/boot/uImage.xz  $PROJ_KERNEL_RELDIR
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
    ;;
esac

