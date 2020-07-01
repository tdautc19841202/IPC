#!/bin/sh

set -e

PROJECT_PACKAGE_DIR=$PWD/../project/package

export STRIP=${CROSS_COMPILE}strip

case "$1" in
"")
    make -j8
	$STRIP $PWD/bin/fsckfat
	mkdir -p $PROJECT_PACKAGE_DIR/ext/bin
	mkdir -p $PROJECT_PACKAGE_DIR/ext/lib
	cp $PWD/bin/fsckfat $PROJECT_PACKAGE_DIR/ext/bin
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
    ;;
esac

