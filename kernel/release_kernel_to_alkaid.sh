#!/bin/bash

while getopts "a:c:" opt; do
  case $opt in
    a)
		alkaid_dir=$OPTARG
		;;
    c)
		chip=$OPTARG
		;;
	\?)
	  echo "Invalid option: -$OPTARG" >&2
	  ;;
  esac
done
kernel_dir=$PWD
uImage_dir=${kernel_dir}/arch/arm/boot
LD_ADDR=0x20008000
MKIMAGE_BIN=scripts/mkimage
MZ_BIN=scripts/mz
TOOL_DIR=scripts/dtc

function replace_dtb_to_kernel()
{
    echo CROSS_COMPILE=$1
    CC=$1
    echo dts-name=$2
    dtb_name=$2
    # build dtb
    ${CC}gcc -E -nostdinc -I${uImage_dir}/dts -I${uImage_dir}/dts/include -undef -D__DTS__ -x assembler-with-cpp -o ${uImage_dir}/dts/${dtb_name}.dtb.tmp ${uImage_dir}/dts/${dtb_name}.dts
    ${TOOL_DIR}/dtc -O dtb -o ${uImage_dir}/dts/${dtb_name}.dtb -b 0 ${uImage_dir}/dts/${dtb_name}.dtb.tmp
    python scripts/ms_builtin_dtb_update.py ${uImage_dir}/Image ${uImage_dir}/dts/${dtb_name}.dtb
    IMGNAME=$(strings -a -T binary ${uImage_dir}/Image | grep 'MVX' | grep 'LX'  | sed 's/\\*MVX/MVX/g' | cut -c 1-32)
    # uImage
    ${MKIMAGE_BIN} -A arm -O linux -T kernel -C none -a ${LD_ADDR} -e ${LD_ADDR} -n $${IMGNAME} -d ${uImage_dir}/Image ${uImage_dir}/uImage
    # xz Image
    xz -z -k -f ${uImage_dir}/Image
    ${MKIMAGE_BIN} -A arm -O linux -C lzma -a ${LD_ADDR} -e ${LD_ADDR} -n $${IMGNAME} -d ${uImage_dir}/Image.xz ${uImage_dir}/uImage.xz
    # mz Image
    ${MZ_BIN} c ${uImage_dir}/Image ${uImage_dir}/Image.mz
    ${MKIMAGE_BIN} -A arm -O linux -C mz -a ${LD_ADDR} -e ${LD_ADDR} -n $${IMGNAME} -d ${uImage_dir}/Image.mz ${uImage_dir}/uImage.mz
}


if [ "${chip}" = "i5" ]
then

	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
	echo "infinity5_ssc007a_s01a_spinand_defconfig"
    whereis ${CROSS_COMPILE}gcc
	make infinity5_ssc007a_s01a_spinand_defconfig
	make clean;make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p ipc -f spinand -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007a_s01a_defconfig"
	make infinity5_ssc007a_s01a_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p ipc -f nor -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007b_s01b_spinand_defconfig"
	make infinity5_ssc007b_s01b_spinand_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007B -p ipc -f spinand -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007b_s01b_defconfig"
	make infinity5_ssc007b_s01b_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007B -p ipc -f nor -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007a_s01a_spinand_uvc_defconfig"
	make infinity5_ssc007a_s01a_spinand_uvc_defconfig
	make clean;make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p usb_cam -f spinand -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007a_s01a_uvc_defconfig"
	make infinity5_ssc007a_s01a_uvc_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p usb_cam -f nor -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007b_s01b_spinand_uvc_defconfig"
	make infinity5_ssc007b_s01b_spinand_uvc_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007B -p usb_cam -f spinand -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007b_s01b_uvc_defconfig"
	make infinity5_ssc007b_s01b_uvc_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007B -p usb_cam -f nor -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007a_s01a_coprocessor_defconfig"
	make infinity5_ssc007a_s01a_coprocessor_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p cop -f nor -o glibc
	cd ${kernel_dir}
	make clean

	echo "infinity5_ssc007a_s01a_spinand_coprocessor_defconfig"
	make infinity5_ssc007a_s01a_spinand_coprocessor_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007A -p cop -f spinand -o glibc
	cd ${kernel_dir}
	make clean

	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"
    whereis ${CROSS_COMPILE}gcc
	echo "infinity5_ssc007b_s01b_defconfig"
	make infinity5_ssc007b_s01b_defconfig
	make -j2
	cd ${alkaid_dir}/project/kbuild/4.9.84/
	./release.sh -k ${kernel_dir} -b 007B -p ipc -f nor -o uclibc
	cd ${kernel_dir}
fi
if [ "${chip}" = "i6" ]
then

    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_defconfig
    make infinity6_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f nor -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_uvc_defconfig
    make infinity6_uvc_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p usbcam -f nor -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_uvc_fastboot_defconfig
    make infinity6_uvc_fastboot_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A-fastboot -p usbcam -f nor -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_defconfig
    make infinity6_ssc009b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f nor -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_spinand_defconfig
    make infinity6_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f spinand -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_spinand_uvc_defconfig
    make infinity6_spinand_uvc_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p usbcam -f spinand -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_spinand_defconfig
    make infinity6_ssc009b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f spinand -c i6 -l glibc -v 8.2.1
    cd ${kernel_dir}

    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_defconfig
    make infinity6_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f nor -c i6 -l uclibc -v 4.9.4
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_uvc_defconfig
    make infinity6_uvc_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p usbcam -f nor -c i6 -l uclibc -v 4.9.4
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_uvc_fastboot_defconfig
    make infinity6_uvc_fastboot_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A-fastboot -p usbcam -f nor -c i6 -l uclibc -v 4.9.4
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_defconfig
    make infinity6_ssc009b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f nor -c i6 -l uclibc -v 4.9.4
    cd ${kernel_dir}

    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
    declare -x PATH="/tools/toolchain/arm-linux-gnueabihf-4.8.3-201404/bin:$PATH"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_defconfig
    make infinity6_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f nor -c i6 -l glibc -v 4.8.3
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_defconfig
    make infinity6_ssc009b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f nor -c i6 -l glibc -v 4.8.3
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_spinand_defconfig
    make infinity6_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f spinand -c i6 -l glibc -v 4.8.3
    cd ${kernel_dir}

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_spinand_defconfig
    make infinity6_ssc009b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f spinand -c i6 -l glibc -v 4.8.3
    cd ${kernel_dir}
fi

if [ "${chip}" = "i6_dualos" ]
then
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"

    # SPI_NAND 009A
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009a_s01a_spinand_lh_defconfig
    make infinity6_ssc009a_s01a_spinand_lh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc-rtos -f spinand -c i6 -l glibc -v 8.2.1 -i 4.9.84
    cd ${kernel_dir}

    # SPI_NAND Ramdisk 009A
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009a_s01a_lh_defconfig
    make infinity6_ssc009a_s01a_lh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc-rtos -f spinand-ramdisk -c i6 -l glibc -v 8.2.1 -i 4.9.84
    cd ${kernel_dir}

    # SPI_NAND Ramdisk 009B
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    echo make infinity6_ssc009b_s01a_lh_defconfig
    make infinity6_ssc009b_s01a_lh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc-rtos -f spinand-ramdisk -c i6 -l glibc -v 8.2.1 -i 4.9.84
    cd ${kernel_dir}
fi

if [ "${chip}" = "i2m" ]
then
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
    echo GCC_VERSION=${GCC_VERSION}

    # NVR SPI_NAND
    echo "infinity2m_spinand_ssc010a_s01a_defconfig"
    make infinity2m_spinand_ssc010a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A -p nvr -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # NVR SPI_NOR
    echo "infinity2m_ssc010a_s01a_defconfig"
    make infinity2m_ssc010a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A -p nvr -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # NVR SPI_NAND
    echo "infinity2m_spinand_ssc010a_ssr623_s01a_defconfig"
    make infinity2m_spinand_ssc010a_ssr623_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A-623 -p nvr -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # NVR SPI_NOR
    echo "infinity2m_ssc010a_ssr623_s01a_defconfig"
    make infinity2m_ssc010a_ssr623_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A-623 -p nvr -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}


    # P2 SPI_NAND
    echo "infinity2m_spinand_ssc011a_s01a_defconfig"
    make infinity2m_spinand_ssc011a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A -p nvr -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # P2 SPI_NOR
    echo "infinity2m_ssc011a_s01a_defconfig"
    make infinity2m_ssc011a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A -p nvr -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}


    echo "infinity2m_ssc011a_s01a_fastboot_defconfig"
    make infinity2m_ssc011a_s01a_fastboot_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A-fastboot -p nvr -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    echo "infinity2m_ssc011a_s01a_minigui_defconfig"
    make infinity2m_ssc011a_s01a_minigui_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A-cus -p nvr -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    echo "infinity2m_spinand_ssc011a_s01a_minigui_defconfig"
    make infinity2m_spinand_ssc011a_s01a_minigui_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A-cus -p nvr -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
    echo GCC_VERSION=${GCC_VERSION}

    # NVR SPI_NAND
    echo "infinity2m_spinand_ssc010a_s01a_defconfig"
    make infinity2m_spinand_ssc010a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A -p nvr -f spinand -c ${chip} -l uclibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # NVR SPI_NOR
    echo "infinity2m_ssc010a_s01a_defconfig"
    make infinity2m_ssc010a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A -p nvr -f nor -c ${chip} -l uclibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # P2 SPI_NAND
    echo "infinity2m_spinand_ssc011a_s01a_defconfig"
    make infinity2m_spinand_ssc011a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A -p nvr -f spinand -c ${chip} -l uclibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # P2 SPI_NOR
    echo "infinity2m_ssc011a_s01a_defconfig"
    make infinity2m_ssc011a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 011A -p nvr -f nor -c ${chip} -l uclibc -v ${GCC_VERSION}
    cd ${kernel_dir}

fi

if [ "${chip}" = "i2m-dualos-amp" ]
then
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
    echo GCC_VERSION=${GCC_VERSION}

    # NVR SPI_NAND
    echo "infinity2m_spinand_ssc010a_s01a_swtoe_defconfig"
    make infinity2m_spinand_ssc010a_s01a_swtoe_defconfig 
    #echo "infinity2m_spinand_ssc010a_s01a_amp_defconfig"
    #make infinity2m_spinand_ssc010a_s01a_amp_defconfig 

    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 010A -p nvr-rtos -f spinand -c i2m -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}
fi

if [ "${chip}" = "i6e" ]
then
    # infinity6e_ssc012b_s01a_defconfig  --> QFN for nor
    # infinity6e_ssc013a_s01a_defconfig  -> BGA for nor
    # infinity6e_ssc012b_s01a_spinand_defconfig -> QFN for spinand
    # infinity6e_ssc013a_s01a_spinand_defconfig -> BGA for spinand
    # infinity6e_ssc012b_s01a_spinand_amp_defconfig -> dualOS QFN for spinand
    # infinity6e_ssc013a_s01a_spinand_amp_defconfig -> dualOS BGA for spinand

    ########################## gclibc 8.2.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=8.2.1
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NOR
    echo make infinity6e_ssc012b_s01a_defconfig
    make infinity6e_ssc012b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NOR
    echo make infinity6e_ssc013a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NOR
    echo make infinity6e_ssc015a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_defconfig
    make infinity6e_ssc012b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NAND
    echo make infinity6e_ssc015a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ##### for usbcam
    # 012B SPI_NOR
    echo make infinity6e_ssc012b_s01a_usbcam_defconfig
    make infinity6e_ssc012b_s01a_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NOR
    echo make infinity6e_ssc013a_s01a_usbcam_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_usbcam_defconfig
    make infinity6e_ssc012b_s01a_spinand_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_usbcam_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ########################## gclibc 9.1.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-9.1.0-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=9.1.0
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NOR
    echo make infinity6e_ssc012b_s01a_defconfig
    make infinity6e_ssc012b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NOR
    echo make infinity6e_ssc013a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NOR
    echo make infinity6e_ssc015a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_defconfig
    make infinity6e_ssc012b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NAND
    echo make infinity6e_ssc015a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ##### for usbcam
    # 012B SPI_NOR
    echo make infinity6e_ssc012b_s01a_usbcam_defconfig
    make infinity6e_ssc012b_s01a_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NOR
    echo make infinity6e_ssc013a_s01a_usbcam_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam -f nor -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_usbcam_defconfig
    make infinity6e_ssc012b_s01a_spinand_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_usbcam_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam -f spinand -c ${chip} -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ########################## uclibc ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NOR
    echo make infinity6e_ssc012b_s01a_defconfig
    make infinity6e_ssc012b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f nor -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NOR
    echo make infinity6e_ssc013a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f nor -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NOR
    echo make infinity6e_ssc015a_s01a_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f nor -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}


    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_defconfig
    make infinity6e_ssc012b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc -f spinand -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc013a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc -f spinand -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 015A SPI_NAND
    echo make infinity6e_ssc015a_s01a_spinand_defconfig
    replace_dtb_to_kernel ${CROSS_COMPILE} infinity6e-ssc015a-s01a
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 015A -p ipc -f spinand -c ${chip} -l uclibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}
fi

if [ "${chip}" = "i6e-dualos-amp" ]
then
    # infinity6e_ssc013a_s01a_spinand_amp_defconfig -> dualOS BGA for spinand
    # infinity6e_ssc012b_s01a_spinand_amp_defconfig -> dualOS QFN for spinand

    ########################## gclibc 8.2.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=8.2.1
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_amp_defconfig
    make infinity6e_ssc012b_s01a_spinand_amp_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_amp_defconfig
    make infinity6e_ssc013a_s01a_spinand_amp_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ##### for usbcam
    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_amp_usbcam_defconfig
    make infinity6e_ssc012b_s01a_spinand_amp_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_amp_usbcam_defconfig
    make infinity6e_ssc013a_s01a_spinand_amp_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}


    ########################## gclibc 9.1.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-9.1.0-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=9.1.0
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_amp_defconfig
    make infinity6e_ssc012b_s01a_spinand_amp_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_amp_defconfig
    make infinity6e_ssc013a_s01a_spinand_amp_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ##### for usbcam
    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_amp_usbcam_defconfig
    make infinity6e_ssc012b_s01a_spinand_amp_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p usbcam-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_amp_usbcam_defconfig
    make infinity6e_ssc013a_s01a_spinand_amp_usbcam_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p usbcam-rtos -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}
fi

if [ "${chip}" = "i6e-dualos-smplh" ]
then
    # infinity6e_ssc013a_s01a_spinand_smplh_defconfig -> dualOS BGA for spinand
    # infinity6e_ssc012b_s01a_spinand_smplh_defconfig -> dualOS QFN for spinand

    ########################## gclibc 8.2.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=8.2.1
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_smplh_defconfig
    make infinity6e_ssc012b_s01a_spinand_smplh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc-rtos-smplh -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_smplh_defconfig
    make infinity6e_ssc013a_s01a_spinand_smplh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc-rtos-smplh -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    ########################## gclibc 9.1.0 ###############################
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-9.1.0-"

    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=9.1.0
    echo GCC_VERSION=${GCC_VERSION}

    # 012B SPI_NAND
    echo make infinity6e_ssc012b_s01a_spinand_smplh_defconfig
    make infinity6e_ssc012b_s01a_spinand_smplh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 012B -p ipc-rtos-smplh -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}

    # 013A SPI_NAND
    echo make infinity6e_ssc013a_s01a_spinand_smplh_defconfig
    make infinity6e_ssc013a_s01a_spinand_smplh_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 013A -p ipc-rtos-smplh -f spinand -c i6e -l glibc -v ${GCC_VERSION} -i 4.9.84
    cd ${kernel_dir}
fi

if [ "${chip}" = "i6b0" ]
then
    declare -x PATH=/tools/toolchain/gcc-sigmastar-9.1.0-2019.11-x86_64_arm-linux-gnueabihf/bin/:$PATH
    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-linux-gnueabihf-9.1.0-"
    echo CROSS_COMPILE=$CROSS_COMPILE
    whereis ${CROSS_COMPILE}gcc
    GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
    echo GCC_VERSION=${GCC_VERSION}
    # qfn88  009A SPI_NOR
    echo "infinity6b0_ssc009a_s01a_defconfig"
    make infinity6b0_ssc009a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # qfn88  009A SPI_NAND
    echo "infinity6b0_ssc009a_s01a_spinand_defconfig"
    make infinity6b0_ssc009a_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # qfn128  009B SPI_NOR
    echo "infinity6b0_ssc009b_s01a_defconfig"
    make infinity6b0_ssc009b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f nor -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    # qfn128  009B SPI_NAND
    echo "infinity6b0_ssc009b_s01a_spinand_defconfig"
    make infinity6b0_ssc009b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f spinand -c ${chip} -l glibc -v ${GCC_VERSION}
    cd ${kernel_dir}

    declare -x ARCH="arm"
    declare -x CROSS_COMPILE="arm-buildroot-linux-uclibcgnueabihf-"

    # qfn88  009A SPI_NOR
    echo "infinity6b0_ssc009a_s01a_defconfig"
    make infinity6b0_ssc009a_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f nor -c ${chip} -l uclibc -v 4.9.4
    cd ${kernel_dir}

    echo "infinity6b0_ssc009a_s01a_spinand_defconfig"
    make infinity6b0_ssc009a_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009A -p ipc -f spinand -c ${chip} -l uclibc -v 4.9.4
    cd ${kernel_dir}

    # qfn128  009B SPI_NOR
    echo "infinity6b0_ssc009b_s01a_defconfig"
    make infinity6b0_ssc009b_s01a_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f nor -c ${chip} -l uclibc -v 4.9.4
    cd ${kernel_dir}

    # qfn128  009B SPI_NAND
    echo "infinity6b0_ssc009b_s01a_spinand_defconfig"
    make infinity6b0_ssc009b_s01a_spinand_defconfig
    make clean;make -j2
    cd ${alkaid_dir}/project/kbuild/4.9.84/
    ./release.sh -k ${kernel_dir} -b 009B -p ipc -f spinand -c ${chip} -l uclibc -v 4.9.4
    cd ${kernel_dir}
fi
