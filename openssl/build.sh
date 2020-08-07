#!/bin/sh

set -e

APICAL_RELEASE_DIR=$PWD/../apical/release

 export CC=gcc

case "$1" in
"")
    if [ ! -e $PWD/Makefile ]; then
        ./config shared --prefix=$PWD/_install
    fi
    make -j8
    make install_sw
    chmod 644 $PWD/_install/lib/*.so*
    mkdir -p $APICAL_RELEASE_DIR/lib
    cp $PWD/_install/lib/libcrypto.so.1.0.0 $APICAL_RELEASE_DIR/lib
    cp $PWD/_install/lib/libssl.so.1.0.0    $APICAL_RELEASE_DIR/lib
    ;;
clean)
    rm -rf $PWD/_install
    make clean
    ;;
distclean)
    rm -rf $PWD/_install
    make distclean
    ;;
esac
