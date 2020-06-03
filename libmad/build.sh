#!/bin/sh

set -e

APICAL_RELEASE_DIR=$PWD/../apical/release

case "$1" in
"")
    if [ ! -e $PWD/Makefile ]; then
        ./configure \
        --host=arm-buildroot-linux-uclibcgnueabihf \
        --prefix=$PWD/_install \
        --enable-speed \
        --enable-fpm=arm \
        --enable-sso \
        --disable-debugging
        touch aclocal.m4
        touch configure
        touch Makefile.in
    fi
    make -j8
    make install
    mkdir -p $APICAL_RELEASE_DIR/lib
    cp $PWD/_install/lib/libmad.so.0 $APICAL_RELEASE_DIR/lib
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    rm -rf $PWD/_install
    rm -rf $PWD/Makefile
    ;;
esac

