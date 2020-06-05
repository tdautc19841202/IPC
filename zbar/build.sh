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
        --disable-static \
        --disable-video \
        --with-libiconv-prefix=$PWD/../libiconv/_install/ \
        --without-x \
        --without-xshm \
        --without-xv \
        --without-jpeg \
        --without-imagemagick \
        --without-gtk \
        --without-python \
        --without-qt
        touch aclocal.m4
        touch configure
        touch Makefile.in
    fi
    make -j8
    make install
	$STRIP $PWD/_install/lib/libzbar.so.0
	cp $PWD/_install/lib/libzbar.so.0 $PROJECT_PACKAGE_DIR/ext/lib
    ;;
clean)
    make clean
    ;;
distclean)
    rm -rf $PWD/_install
    make distclean
    ;;
esac
