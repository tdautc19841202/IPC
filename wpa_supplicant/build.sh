#!/bin/sh

set -e

PROJECT_RELEASE_DIR=$PWD/../project/package/ext
LIBNL_INSTALL_DIR=$PWD/../libnl/_install
OPENSSL_INSTALL_DIR=$PWD/../openssl/_install

export CC=arm-buildroot-linux-uclibcgnueabihf-gcc
export PKG_CONFIG_PATH=$LIBNL_INSTALL_DIR/lib/pkgconfig:$OPENSSL_INSTALL_DIR/lib/pkgconfig
export EXTRA_CFLAGS="-I$LIBNL_INSTALL_DIR/include -I$LIBNL_INSTALL_DIR/../include -I$OPENSSL_INSTALL_DIR/include"
export LIBS="-L$LIBNL_INSTALL_DIR/lib -L$OPENSSL_INSTALL_DIR/lib"

case "$1" in
"")
    if [ ! -e $PWD/wpa_supplicant/.config ]; then
        cp $PWD/wpa_supplicant/defconfig $PWD/wpa_supplicant/.config 
    fi
    make -C $PWD/wpa_supplicant -j8
    arm-buildroot-linux-uclibcgnueabihf-strip $PWD/wpa_supplicant/wpa_supplicant
    arm-buildroot-linux-uclibcgnueabihf-strip $PWD/wpa_supplicant/wpa_cli
	arm-buildroot-linux-uclibcgnueabihf-strip $PWD/wpa_supplicant/wpa_passphrase

    mkdir -p $PROJECT_RELEASE_DIR/bin
    cp $PWD/wpa_supplicant/wpa_supplicant $PROJECT_RELEASE_DIR/bin
    cp $PWD/wpa_supplicant/wpa_cli        $PROJECT_RELEASE_DIR/bin
	cp $PWD/wpa_supplicant/wpa_passphrase $PROJECT_RELEASE_DIR/bin
    ;;
clean)
    make -C $PWD/wpa_supplicant clean
    ;;
distclean)
    make -C $PWD/wpa_supplicant clean
    rm $PWD/wpa_supplicant/.config
    ;;
esac

