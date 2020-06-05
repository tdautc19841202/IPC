#!/bin/sh

set -e

PROJECT_RELEASE_DIR=$PWD/../project/package/ext
LIBNL_INSTALL_DIR=$PWD/../libnl/_install
OPENSSL_INSTALL_DIR=$PWD/../openssl/_install

export CC=arm-buildroot-linux-uclibcgnueabihf-gcc
export PKG_CONFIG_PATH=$LIBNL_INSTALL_DIR/lib/pkgconfig:$OPENSSL_INSTALL_DIR/lib/pkgconfig
export EXTRA_CFLAGS="-I$LIBNL_INSTALL_DIR/include -I$OPENSSL_INSTALL_DIR/include"
export LIBS="-L$LIBNL_INSTALL_DIR/lib -L$OPENSSL_INSTALL_DIR/lib"

case "$1" in
"")
    if [ ! -e $PWD/hostapd/.config ]; then
        cp $PWD/hostapd/defconfig $PWD/hostapd/.config 
    fi
    make -C $PWD/hostapd -j8
    arm-buildroot-linux-uclibcgnueabihf-strip   $PWD/hostapd/hostapd
    arm-buildroot-linux-uclibcgnueabihf-strip   $PWD/hostapd/hostapd_cli
    cp $PWD/hostapd/hostapd     $PROJECT_RELEASE_DIR/bin
#   cp $PWD/hostapd/hostapd_cli $PROJECT_RELEASE_DIR/bin
    ;;
clean)
    make -C $PWD/hostapd clean
    ;;
distclean)
    make -C $PWD/hostapd clean
    rm $PWD/hostapd/.config
    ;;
esac

