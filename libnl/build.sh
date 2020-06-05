#!/bin/sh

set -e

APICAL_RELEASE_DIR=$PWD/../apical/release

case "$1" in
"")
    if [ ! -f Makefile ]; then
        ./configure --prefix=$PWD/_install --host=arm-buildroot-linux-uclibcgnueabihf --disable-debug --disable-cli
		touch configure.ac aclocal.m4 configure Makefile.am Makefile.in
		touch ./lib/defs.h.in
    fi
    make -j8
    make install
    mkdir -p $APICAL_RELEASE_DIR/lib/
    cp $PWD/_install/lib/libnl-3.so.200      $APICAL_RELEASE_DIR/lib/
    cp $PWD/_install/lib/libnl-genl-3.so.200 $APICAL_RELEASE_DIR/lib/
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    rm -rf $PWD/_install 
    ;;
esac
