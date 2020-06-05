#!/bin/sh

set -e

export KSRC=$PWD/../kernel
export KVER=3.18.30

case "$1" in
"")
    make -j8
    arm-buildroot-linux-uclibcgnueabihf-strip -d $PWD/8188fu.ko
    cp $PWD/8188fu.ko $PWD/../apical/8188fu/
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
    ;;
esac

