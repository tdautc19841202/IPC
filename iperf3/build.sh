#!/bin/sh

set -e

PROJECT_PACKAGE_DIR=$PWD/../project/package

export STRIP=${CROSS_COMPILE}strip

case "$1" in
"")
    if [ ! -f Makefile ]; then
        ./configure --prefix=$PWD/_install --host=arm-buildroot-linux-uclibcgnueabihf
        touch aclocal.m4
        touch configure
        touch Makefile.in
    fi
    make -j8
    make install

	$STRIP $PWD/_install/lib/libiperf.so.0
	$STRIP $PWD/_install/bin/iperf3
	cp $PWD/_install/lib/libiperf.so.0 $PROJECT_PACKAGE_DIR/ext/lib/
	cp $PWD/_install/bin/iperf3        $PROJECT_PACKAGE_DIR/ext/bin/
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    rm -rf $PWD/_install 
    ;;
esac
