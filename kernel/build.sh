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
    #cp $PWD/arch/arm/boot/uImage.mz  $PROJ_KERNEL_RELDIR
    #cp $PWD/arch/arm/boot/uImage.xz  $PROJ_KERNEL_RELDIR
	cp arch/arm/boot/uImage.xz ../project/release/ipc/i6b0/009A/uclibc/4.9.4/bin/kernel/nor/
	for file in $(find $PWD/modules/ -name "*.ko"); do
        arm-buildroot-linux-uclibcgnueabihf-strip -d $file
    done
	cp -rf modules/* ../project/kbuild/4.9.84/i6b0/configs/ipc/009A/uclibc/4.9.4/nor/modules/
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    ;;
esac