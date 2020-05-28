#!/bin/sh

set -e

APICAL_RELEASE_DIR=$PWD/release
PROJECT_PACKAGE_DIR=$PWD/../project/package/ext

export STRIP=${CROSS_COMPILE}strip

HWINFOSTR_KPJ_="R_76|G_77|U_77|LR0|FL3|WI14|SP15|IR61|CA78|CB79|V_2|M_0|L_0"
HWINFOSTR_YTJ_="R_76|G_77|U_77|LR0|FL3|WI14|SP15|IR61|CA78|CB79|V_2|M_0|L_0"
HWINFOSTR_DENG="R_61|G_62|U_62|LR1|FL0|WI98|SP99|IR-1|CA-1|CB-1|V_2|M_0|L_1|IQ0"

case "$1" in
"")
	# build apkapi
	make -C $PWD/apkapi
	
	#build MI_package
	make -C $PWD/MI_package

	# build librtsp
	make -C $PWD/librtsp

	# build ipcam
	make -C $PWD/ipcam
	mkdir -p $APICAL_RELEASE_DIR/bin
	cp $PWD/ipcam/bin/ipcam $APICAL_RELEASE_DIR/bin

	# build rtwpriv
	make -C $PWD/rtwpriv
	$STRIP $PWD/rtwpriv/bin/rtwpriv
	cp $PWD/rtwpriv/bin/rtwpriv $PROJECT_PACKAGE_DIR/bin

	# copy wifi driver files
	mkdir -p $APICAL_RELEASE_DIR/lib/modules/
	cp $PWD/8188fu/*.sh $APICAL_RELEASE_DIR/bin

	# copy helper script files
	cp $PWD/ipcam/init.sh	  $APICAL_RELEASE_DIR/bin
	cp $PWD/ipcam/watchdog.sh $APICAL_RELEASE_DIR/bin

	#copy sd hotplug file
	cp $PWD/tools/sd_hotplug/*.sh $APICAL_RELEASE_DIR/bin

	# copy res files
	cp -r $PWD/res $APICAL_RELEASE_DIR

	# copy debuglog tool
	cp $PWD/tools/debuglog/debuglog $PROJECT_PACKAGE_DIR/bin

	# delete fw_setenv
	rm -rf $APICAL_RELEASE_DIR/bin/fw_setenv

	# strip binary files
	for file in $APICAL_RELEASE_DIR/lib/*; do
		if [ x"${file}" != x"$APICAL_RELEASE_DIR/lib/modules" ]; then
			$STRIP $file
			cp $file $PROJECT_PACKAGE_DIR/lib
		fi
	done
	for file in $APICAL_RELEASE_DIR/bin/*; do
		if [ x"${file##*.}" != x"sh" ]; then
			$STRIP $file
		fi
	done

	;;
clean)
	make -C $PWD/apkapi		clean
	make -C $PWD/MI_package clean
	make -C $PWD/librtsp	clean
	make -C $PWD/libuvc		clean
	make -C $PWD/ipcam		clean
	make -C $PWD/rtwpriv	clean
	;;
distclean)
	make -C $PWD/apkapi		clean
	make -C $PWD/MI_package clean
	make -C $PWD/librtsp	clean
	make -C $PWD/libuvc		clean
	make -C $PWD/ipcam		clean
	make -C $PWD/rtwpriv	clean
	rm -rf $PWD/release
	;;
update-all)
	rm -rf $PWD/update-all.tar.gz
	cp -r $PWD/tools/update $PWD
	cp $PWD/../project/image/output/images/rootfs.sqfs	   $PWD/update
	cp $PWD/../project/image/output/images/nvrservice.sqfs $PWD/update
	cp $PWD/../project/image/output/images/customer.jffs2  $PWD/update
	tar czf update-all.tar.gz update
	rm -rf $PWD/update
	;;
update-customer)
	rm -rf $PWD/update-customer.tar.gz
	cp -r $PWD/tools/update $PWD
	cp $PWD/../project/image/output/images/customer.jffs2  $PWD/update
	tar czf update-customer.tar.gz update
	rm -rf $PWD/update
	;;
update-app)
	cp -r  $PWD/tools/update $PWD
	cp -r  $PWD/release $PWD/update/app
	rm -f  $PWD/update/flash_erase
	rm -f  $PWD/update/nandwrite
	rm -rf $PWD/update/app/lib
	tar czf update-app-$(date +%y%m%d%H%M).tar.gz update
	rm -rf $PWD/update
	;;
flash)
	cp $PWD/../project/one.bin $PWD/tools/fw_setenv/
	case "$2" in
	"")
		echo "usage: ./build.sh flash kpj|ytj|deng"
		exit
		;;
	kpj)
		cd $PWD/tools/fw_setenv
		./setupenv.sh $HWINFOSTR_KPJ_ 2> /dev/null
		cd - > /dev/null
		;;
	ytj)
		cd $PWD/tools/fw_setenv
		./setupenv.sh $HWINFOSTR_YTJ_ 2> /dev/null
		cd - > /dev/null
		;;
	deng)
		cd $PWD/tools/fw_setenv
		./setupenv.sh $HWINFOSTR_DENG 2> /dev/null
		cd - > /dev/null
		;;
	esac
	mv $PWD/tools/fw_setenv/one.bin $PWD
	echo "build one.bin ok !"
	echo "->" `ls $PWD/one.bin`
	;;
esac
