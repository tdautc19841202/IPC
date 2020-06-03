#!/bin/sh

set -e

PROJECT_PACKAGE_DIR=$PWD/../project/package

export STRIP=${CROSS_COMPILE}strip
export PREFIX=$PWD/_install
CC=arm-buildroot-linux-uclibcgnueabihf-gcc
case "$1" in
"")
    # make -j8
    make CC=arm-buildroot-linux-uclibcgnueabihf-gcc -j8
    make install
	$STRIP $PWD/_install/sbin/*
	$STRIP $PWD/_install/lib/libiw.so.30
	cp $PWD/_install/sbin/*            $PROJECT_PACKAGE_DIR/ext/bin/
	cp $PWD/_install/lib/libiw.so.30   $PROJECT_PACKAGE_DIR/ext/lib/
    ;;
clean)
    make clean
    ;;
distclean)
    rm -rf $PWD/_install
    make clean
    ;;
esac
