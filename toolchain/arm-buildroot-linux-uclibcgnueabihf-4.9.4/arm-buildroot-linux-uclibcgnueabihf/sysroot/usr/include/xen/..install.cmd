cmd_/home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen/.install := /bin/sh scripts/headers_install.sh /home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen ./include/uapi/xen evtchn.h gntalloc.h gntdev.h privcmd.h; /bin/sh scripts/headers_install.sh /home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen ./include/xen ; /bin/sh scripts/headers_install.sh /home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen ./include/generated/uapi/xen ; for F in ; do echo "\#include <asm-generic/$$F>" > /home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen/$$F; done; touch /home/karl.xiao/buildroot-2017.08/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include/xen/.install
