#!/bin/sh

make clean;
declare -x ARCH="arm";
declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-";
make infinity6b0_ssc009a_s01a_defconfig;
make -j8;
cp arch/arm/boot/uImage.xz ../project/release/ipc/i6b0/009A/uclibc/4.9.4/bin/kernel/nor/;
cp -rf modules/* ../project/kbuild/4.9.84/i6b0/configs/ipc/009A/uclibc/4.9.4/nor/modules/;