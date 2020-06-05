#!/bin/sh

set -e

ZLIB_INSTALL_DIR=$PWD/../zlib/_install

export CROSS=arm-buildroot-linux-uclibcgnueabihf-
export DESTDIR=$PWD/_install
export ZLIBCPPFLAGS="-I$ZLIB_INSTALL_DIR/include"
export ZLIBLDFLAGS="-L$ZLIB_INSTALL_DIR/lib"
export STRIP=${CROSS}strip

case "$1" in
"")
    make -j8 WITHOUT_XATTR=1 WITHOUT_LZO=1
    make install
    for file in $PWD/_install/usr/sbin/*; do
        if [ x"$file" != x"$PWD/_install/usr/sbin/flash_eraseall" ]; then
            $STRIP $file
        fi
    done
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
    rm -rf $DESTDIR
    ;;
esac

