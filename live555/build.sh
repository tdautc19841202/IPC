#!/bin/sh

set -e

export DESTDIR=$PWD/_install

case "$1" in
"")
    if [ ! -f Makefile ]; then
        ./genMakefiles msc3xx
    fi
    make -j8
    make install
    ;;
clean)
    make clean
    ;;
distclean)
    make distclean
    ;;
esac
