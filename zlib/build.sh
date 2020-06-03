#!/bin/sh

set -e

APICAL_RELEASE_DIR=$PWD/../apical/release

export CC=arm-buildroot-linux-uclibcgnueabihf-gcc

case "$1" in
"")
    if [ ! -e $PWD/Makefile ]; then
        ./configure --prefix=$PWD/_install
    fi
    make -j8
    make install
    mkdir -p $APICAL_RELEASE_DIR/lib
    cp $PWD/_install/lib/libz.so.1 $APICAL_RELEASE_DIR/lib
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
