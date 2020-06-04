#!/bin/sh

set -e

TOPDIR=$PWD

	cd $TOPDIR/boot           && ./build.sh $1
	cd $TOPDIR/kernel         && ./build.sh $1
	#cd $TOPDIR/busybox        && ./build.sh $1
	#cd $TOPDIR/zlib           && ./build.sh $1
	#cd $TOPDIR/mtd-utils      && ./build.sh $1
	#cd $TOPDIR/fsckfat        && ./build.sh $1
	#cd $TOPDIR/openssl        && ./build.sh $1
	#cd $TOPDIR/libnl          && ./build.sh $1
	#cd $TOPDIR/wpa_supplicant && ./build.sh $1
	#cd $TOPDIR/rtl8188fu      && ./build.sh $1
	#cd $TOPDIR/rtlwifitool    && ./build.sh $1
	#cd $TOPDIR/libiconv       && ./build.sh $1
	#cd $TOPDIR/zbar           && ./build.sh $1
	#cd $TOPDIR/live555        && ./build.sh $1
	#cd $TOPDIR/iperf3         && ./build.sh $1
	#cd $TOPDIR/libmad         && ./build.sh $1
	#cd $TOPDIR/apical         && ./build.sh $1
	cd $TOPDIR/project        && ./build.sh $1
	cd $TOPDIR