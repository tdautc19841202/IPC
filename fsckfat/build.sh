#!/bin/sh

set -e

PROJECT_PACKAGE_DIR=$PWD/../project/package

export STRIP=${CROSS_COMPILE}strip

case "$1" in
"")
    make -j8
	$STRIP $PWD/bin/fsckfat
	cp $PWD/bin/fsckfat $PROJECT_PACKAGE_DIR/ext/bin
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
    ;;
esac

