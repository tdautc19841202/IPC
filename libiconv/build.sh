#!/bin/sh

set -e

PROJECT_PACKAGE_DIR=$PWD/../project/package

export STRIP=${CROSS_COMPILE}strip

case "$1" in
"")
    if [ ! -e $PWD/include/config.h ]; then
        ./configure \
        --host=arm-buildroot-linux-uclibcgnueabihf \
        --prefix=$PWD/_install \
        --enable-shared \
        --disable-static
        touch aclocal.m4
        touch configure
        touch Makefile.in
    fi
    make -j8
    make install
    $STRIP $PWD/_install/lib/libiconv.so.2
    $STRIP $PWD/_install/lib/libiconv.so.2.5.0
    cp -d $PWD/_install/lib/libiconv.so.2     $PROJECT_PACKAGE_DIR/ext/lib
    cp    $PWD/_install/lib/libiconv.so.2.5.0 $PROJECT_PACKAGE_DIR/ext/lib
    ;;
clean)
    make clean
    ;;
distclean)
    rm -rf $PWD/_install
    make distclean
    ;;
esac
