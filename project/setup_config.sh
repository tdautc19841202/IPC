#!/bin/sh

PROJ_ROOT=$PWD

if [ "$#" != "1" ]; then
    echo "usage: $0 configs/config.chip"
fi
if [ -e configs ]; then
    echo PROJ_ROOT = $PROJ_ROOT > configs/current.configs
    echo CONFIG_NAME = config_module_list.mk >> configs/current.configs
    echo SOURCE_MK = ../sdk/sdk.mk >> configs/current.configs
	echo "KERNEL_MEMADR = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM phyaddr)" >> configs/current.configs
	echo "KERNEL_MEMLEN = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM size)" >> configs/current.configs
	echo "KERNEL_MEMADR2 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM2 phyaddr)" >> configs/current.configs
	echo "KERNEL_MEMLEN2 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM2 size)" >> configs/current.configs
	echo "KERNEL_MEMADR3 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM3 phyaddr)" >> configs/current.configs
	echo "KERNEL_MEMLEN3 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM3 size)" >> configs/current.configs
	echo "LOGO_ADDR = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) \$(BOOTLOGO_ADDR) miuaddr)" >> configs/current.configs
    cat $1 >> configs/current.configs
    echo "ARCH=arm" >> configs/current.configs
    echo "CROSS_COMPILE=$(sed -n "/TOOLCHAIN_REL/p"  configs/current.configs | awk '{print $3}')" >> configs/current.configs

    c=$(sed -n "/^CHIP/p"  configs/current.configs | awk '{print $3}')

    if [ "$c" = "i5" ]; then
        echo "CHIP_ALIAS = pretzel" >> configs/current.configs
    elif [ "$c" = "i6" ]; then
        echo "CHIP_ALIAS = macaron" >> configs/current.configs
    elif [ "$c" = "i2m" ]; then
        echo "CHIP_ALIAS = taiyaki" >> configs/current.configs
    elif [ "$c" = "p2" ]; then
        echo "CHIP_ALIAS = takoyaki" >> configs/current.configs
    elif [ "$c" = "i6e" ]; then
        echo "CHIP_ALIAS = pudding" >> configs/current.configs
    elif [ "$c" = "i6b0" ]; then
        echo "CHIP_ALIAS = ispahan" >> configs/current.configs
    fi
else
    echo "can't found configs directory!"
fi

cat configs/current.configs
