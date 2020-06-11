#!/bin/sh

case "$1" in
"")
    if [ ! -e .config ]; then
        make busybox_IPCM_defconfig
    fi
    make -j8
    make install
    cd $PWD/_install
    tar czf ../busybox.tar.gz .
    cd -
    mv $PWD/busybox.tar.gz $PWD/../project/image/busybox/busybox-1.20.2-arm-buildroot-linux-uclibcgnueabihif-static.tar.gz
    ;;
menuconfig)
    make menuconfig
    ;;
clean)
    make clean
    ;;
distclean)
    make uninstall
    make distclean
    ;;
esac
