#!/bin/sh

set -e

case "$1" in
"")
    make clean
    ./setup_config.sh configs/ipc/i6b0/nor.uclibc-squashfs.009a.64.qfn88
    make image
	./onebin16M.sh
    ;;
clean)
    make clean
    ;;
distclean)
    make clean
	rm -rf ../apical/release
    ;;
esac
