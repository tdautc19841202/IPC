#!/bin/sh

export ARCH=arm
export PATH=$PWD/toolchain/arm-buildroot-linux-uclibcgnueabihf-4.9.4/bin:$PATH
export CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
export LD_LIBRARY_PATH=$PWD/toolchain/libiconv
