declare -x ARCH="arm"
declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"
make infinity6b0_ssc009b_s01a_defconfig
make clean;
make -j4;
cp arch/arm/boot/uImage.xz ../project/release/ipc/i6b0/009B/uclibc/4.9.4/bin/kernel/nor/uImage.xz  -rf
