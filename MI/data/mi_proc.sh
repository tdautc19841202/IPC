#!/bin/bash

MI_TOP_PATH=/proc/mstar/MI
OSD_PATH="/proc/mstar/MI/OSD/"
VI_PATH="/proc/mstar/MI/VI/"
VENC_PATH="/proc/mstar/MI/VENC/"
ADEC_PATH="/proc/mstar/MI/ADEC/"
AENC_PATH="/proc/mstar/MI/AENC/"
AI_PATH="/proc/mstar/MI/AI/"
AO_PATH="/proc/mstar/MI/AO/"

ISP_PATH="/proc/mstar/ISP/"

IE_PATH="/proc/mstar/algorithm/"


KERNEL_MVHE_PATH="/sys/class/mstar/mvhe/"
KERNEL_MMFE_PATH="/sys/class/mstar/mmfe/"
KERNEL_MSYS_PATH="/sys/class/mstar/msys/"
KERNEL_MVIP_PATH="/sys/class/mstar/mvip/"
KERNEL_MGVSP1_PATH="/sys/class/mstar/mhvsp1/"
KERNEL_MSCLDMA1_PATH="/sys/class/mstar/mscldma1/"
KERNEL_MSCLDMA2_PATH="/sys/class/mstar/mscldma2/"
KERNEL_MSCLDMA3_PATH="/sys/class/mstar/mscldma3/"

OMX_BELA_PATH="/proc/mstar/OMX/BELA/"
OMX_VMFE0_PATH="/proc/mstar/OMX/VMFE0/"
OMX_VMFE1_PATH="/proc/mstar/OMX/VMFE1/"
OMX_VMFE2_PATH="/proc/mstar/OMX/VMFE2/"
OMX_VVHE0_PATH="/proc/mstar/OMX/VVHE0/"
OMX_VVHE1_PATH="/proc/mstar/OMX/VVHE1/"
OMX_VVHE2_PATH="/proc/mstar/OMX/VVHE2/"
OMX_VSRC_PATH="/proc/mstar/OMX/VSRC/"
OMX_VSPL_PATH="/proc/mstar/OMX/VSPL/"

OMX_IJPE_PATH="/proc/mstar/OMX/IJPE/"
OMX_VJPE_PATH="/proc/mstar/OMX/VJPE/"
OMX_ASRC_PATH="/proc/mstar/OMX/ASRC/"
OMX_ASINK_PATH="/proc/mstar/OMX/ASINK/"
OMX_AVQE_PATH="/proc/mstar/OMX/AVQE/"
OMX_ASPL_PATH="/proc/mstar/OMX/ASPL/"
OMX_ADEC0_PATH="/proc/mstar/OMX/ADEC0/"
OMX_ADEC1_PATH="/proc/mstar/OMX/ADEC1/"
OMX_ADEC2_PATH="/proc/mstar/OMX/ADEC2/"
OMX_AENC0_PATH="/proc/mstar/OMX/AENC0/"
OMX_AENC1_PATH="/proc/mstar/OMX/AENC1/"
OMX_AENC2_PATH="/proc/mstar/OMX/AENC2/"

valueVerticalDisplayFuction(){
	for i in "$@"; do
		#echo $i
		if [ ! -d $i ]; then
			name=$(echo $i | sed "s/^.*\///g")
			echo ATTRIBUTE $name:
			cat $i
			echo
		fi
	done
}

valueHorizontalDisplayFuction(){

	printf "%-20s %-8s %-8s\n" ATTRIBUTE VALUE
	for i in "$@"; do
#		echo $i
		if [ ! -d $i ];	then
			name=$(echo $i | sed "s/^.*\///g")
			printf "%-20s %-8s %-s\n" $name  $(cat $i )
		fi
	done
}

patheHorizontalDisplayFunction(){
	valueHorizontalDisplayFuction $(ls $* | sed "s:^:$*/:")
}

pathVerticalDisplayFunction(){
	valueVerticalDisplayFuction $(ls $* | sed "s:^:$*/:")
}


allDirHorizontalDisplayFunction(){
	for i in "$@"; do
		path=$i
#		echo $path
#		attribute=$(echo $i | sed "s/^.*\///g")
		if [ -d $i ];
		then
			echo ======[PROC]$path  START======
			patheHorizontalDisplayFunction  $path
			echo ======[PROC]$path END======
			echo
		else
			name=$(echo $i | sed "s/^.*\///g")
			printf "%-20s %-8s %-s\n" $name  $(cat $i )
		fi
	done
}

allDirVerticalDisplayFunction(){
	for i in "$@"; do
		path=$i
#		echo $path
#		attribute=$(echo $i | sed "s/^.*\///g")
		if [ -d $i ];
		then
			echo ======[PROC]$path  START======
			patheHorizontalDisplayFunction  $path
			echo ======[PROC]$path END======
			echo
		else
			name=$(echo $i | sed "s/^.*\///g")
			printf "%-20s %-8s %-s\n" $name  $(cat $i )
		fi
	done
}


miModuleDisplayFunciton(){
	module_path=$1
	module_name=$2
	channel_name=$3
	cahnnel_number=$4
		echo ========================[PROC]MI:$module_path START========================
		if [ "$cahnnel_number" == "" ]
		then
			#add path:			ls $module_path | sed "s:^:$module_path:"
		  allDirHorizontalDisplayFunction $(ls $module_path | sed "s:^:$module_path:")
		else
		  echo ======[PROC]$channel_name  $CHANNEL START ======
			patheHorizontalDisplayFunction $module_path"$channel_name"$cahnnel_number/
			echo ======[PROC]$channel_name  $CHANNEL END======
		fi
		echo ========================[PROC]MI:$module_path END========================

}

kernelModuleDisplayFunction()
{
	if [ -d $1 ];	then
		echo ========================[PROC]KERNEL:$1 START========================
		pathVerticalDisplayFunction $1
		echo ========================[PROC]KERNEL:$1 END========================
		echo
	fi
}


ieDisplayFunction()
{
	echo ========================[PROC]IE:$IE_PATH START========================
	allDirHorizontalDisplayFunction  $(ls $IE_PATH | sed "s:^:$IE_PATH:")
	echo ========================[PROC]IE:$IE_PATH END========================
}
ispDisplayFunction()
{
	echo ========================[PROC]ISP:$ISP_PATH START========================
	allDirHorizontalDisplayFunction  $(ls $ISP_PATH | sed "s:^:$ISP_PATH:")
	echo ========================[PROC]ISP:$ISP_PATH END========================
}
kernelDisplayFunction()
{
	kernelModuleDisplayFunction $KERNEL_MVHE_PATH
	kernelModuleDisplayFunction $KERNEL_MMFE_PATH
	kernelModuleDisplayFunction $KERNEL_MSYS_PATH
	kernelModuleDisplayFunction $KERNEL_MVIP_PATH
	kernelModuleDisplayFunction $KERNEL_MGVSP1_PATH
	kernelModuleDisplayFunction $KERNEL_MSCLDMA1_PATH
	kernelModuleDisplayFunction $KERNEL_MSCLDMA2_PATH
	kernelModuleDisplayFunction $KERNEL_MSCLDMA3_PATH
}
miDisplayFunciton()
{
case $1 in
	osd)
		miModuleDisplayFunciton $OSD_PATH OSD
	;;
	vi)
		miModuleDisplayFunciton $VI_PATH VI ViChn $2
	;;
	venc)
		miModuleDisplayFunciton $VENC_PATH VENC VeChn $2
	;;
	adec)
		miModuleDisplayFunciton $ADEC_PATH ADEC AdecChn $2
	;;
	aenc)
		miModuleDisplayFunciton $AENC_PATH AENC AeChn $2
	;;
	ai)
		miModuleDisplayFunciton $AI_PATH AI AiChn $2
	;;
	ao)
		miModuleDisplayFunciton $AO_PATH AO AoChn $2
	;;
	all)

		echo ========================[PROC]MI ALL MODULE START========================
		miModuleDisplayFunciton $OSD_PATH OSD
		miModuleDisplayFunciton $VI_PATH VI ViChn $2
		miModuleDisplayFunciton $VENC_PATH VENC VeChn $2
		miModuleDisplayFunciton $ADEC_PATH ADEC AdecChn $2
		miModuleDisplayFunciton $AENC_PATH AENC AeChn $2
		miModuleDisplayFunciton $AI_PATH AI AiChn $2
		miModuleDisplayFunciton $AO_PATH AO AoChn $2
		echo ========================[PROC]MI ALL MODULE END========================
	;;
	*)
		echo "======[PROC]miDisplayFunciton error param======"
	;;
esac
}

omxModuleDisplayFunction()
{
	if [ -d $1 ];	then
		echo ========================[PROC]OMX:$1 START========================
		allDirHorizontalDisplayFunction  $(ls $1 | sed "s:^:$1:")
		echo ========================[PROC]OMX:$1 END========================
	fi
}
omxDisplayFunction()
{
case $1 in
	BELA)
		omxModuleDisplayFunction $OMX_BELA_PATH
	;;
	VSRC)
			omxModuleDisplayFunction $OMX_VSRC_PATH
	;;
	VSPL)
			omxModuleDisplayFunction $OMX_VSPL_PATH
	;;
	IJPE)
			omxModuleDisplayFunction $OMX_IJPE_PATH
	;;
	VJPE)
			omxModuleDisplayFunction $OMX_VJPE_PATH
	;;
	ASRC)
			omxModuleDisplayFunction $OMX_ASRC_PATH
	;;
	ASINK)
			omxModuleDisplayFunction $OMX_ASINK_PATH
	;;
	AVQE)
			omxModuleDisplayFunction $OMX_AVQE_PATH
	;;
	ASPL)
			omxModuleDisplayFunction $OMX_ASPL_PATH
	;;
	ADEC)
		{
			case $2 in
			0)
				omxModuleDisplayFunction $OMX_ADEC0_PATH
				;;
			1)
				omxModuleDisplayFunction $OMX_ADEC1_PATH
			;;
			2)
				omxModuleDisplayFunction $OMX_ADEC2_PATH
			;;
			"")
				omxModuleDisplayFunction $OMX_ADEC0_PATH
				omxModuleDisplayFunction $OMX_ADEC1_PATH
				omxModuleDisplayFunction $OMX_ADEC2_PATH
			;;
			esac
		}
		;;
	AENC)
		{
			case $2 in
			0)
				omxModuleDisplayFunction $OMX_AENC0_PATH
				;;
			1)
				omxModuleDisplayFunction $OMX_AENC1_PATH
			;;
			2)
				omxModuleDisplayFunction $OMX_AENC2_PATH
			;;
			"")
				omxModuleDisplayFunction $OMX_AENC0_PATH
				omxModuleDisplayFunction $OMX_AENC1_PATH
				omxModuleDisplayFunction $OMX_AENC2_PATH
			;;
			esac
		}
		;;
	VMFE)
		{
			case $2 in
			0)
				omxModuleDisplayFunction $OMX_VMFE0_PATH
				;;
			1)
				omxModuleDisplayFunction $OMX_VMFE1_PATH
			;;
			2)
				omxModuleDisplayFunction $OMX_VMFE2_PATH
			;;
			"")
				omxModuleDisplayFunction $OMX_VMFE0_PATH
				omxModuleDisplayFunction $OMX_VMFE1_PATH
				omxModuleDisplayFunction $OMX_VMFE2_PATH
			;;
			esac
		}
		;;
	VVHE)
		{
			case $2 in
			0)
				omxModuleDisplayFunction $OMX_VVHE0_PATH
				;;
			1)
				omxModuleDisplayFunction $OMX_VVHE1_PATH
			;;
			2)
				omxModuleDisplayFunction $OMX_VVHE2_PATH
			;;
			"")
				omxModuleDisplayFunction $OMX_VVHE0_PATH
				omxModuleDisplayFunction $OMX_VVHE1_PATH
				omxModuleDisplayFunction $OMX_VVHE2_PATH
			;;
			esac
		}
	;;
	all)
		echo ========================[PROC]OMX ALL MODULE START========================
		omxDisplayFunction BELA
		omxDisplayFunction VSRC
		omxDisplayFunction VSPL
		omxDisplayFunction IJPE
		omxDisplayFunction VJPE
		omxDisplayFunction VMFE
		omxDisplayFunction VVHE
		omxDisplayFunction ASRC
		omxDisplayFunction ASINK
		omxDisplayFunction AVQE
		omxDisplayFunction ASPL
		omxDisplayFunction ADEC
		omxDisplayFunction AENC
		echo ========================[PROC]OMX ALL MODULE START========================
	;;
	*)
		echo "======error param======"
		helpFunction
	;;
esac
}
helpFunction(){
	echo ========================[PROC]MI proc shell param note========================
	echo "param 1 :module name ,mandatory(sh proc.sh kernel)"
	echo "	main module name:[mi] [omx] [kernel] [isp] [ie] [all]"
	echo "	mi sub module name: [osd] [vi] [venc] [adec] [aenc] [ai] [ao]"
	echo "	omx sub module name:[BELA] [VSRC] [VSPL] [IJPE] [VJPE] [ASRC] [ASINK] [AVQE] [ASPL] [ADEC] [AENC] [VMFE] [VVHE]"
	echo "param 2 :channel number [0-10] ,optional(sh proc.sh vi 0)"
	echo "	support module [vi] [venc] [adec] [aenc] [ai] [ao] [VMFE] [VVHE] [ADEC] [AENC])"
}

main()
{
	case $1 in
		osd)
			miDisplayFunciton $1 $2
			;;
		vi)
			miDisplayFunciton $1 $2
		;;
		venc)
			miDisplayFunciton $1 $2
		;;
		adec)
			miDisplayFunciton $1 $2
		;;
		aenc)
			miDisplayFunciton $1 $2
		;;
		ai)
			miDisplayFunciton $1 $2
		;;
		ao)
			miDisplayFunciton $1 $2
		;;
		kernel)
			kernelDisplayFunction
		;;
		BELA)
			omxDisplayFunction $1
		;;
		VSRC)
			omxDisplayFunction $1
		;;
		VSPL)
			omxDisplayFunction $1
		;;
		IJPE)
			omxDisplayFunction $1
		;;
		VJPE)
			omxDisplayFunction $1
		;;
		ASRC)
			omxDisplayFunction $1
		;;
		ASINK)
			omxDisplayFunction $1
		;;
		AVQE)
			omxDisplayFunction $1
		;;
		ASPL)
			omxDisplayFunction $1
		;;
		ADEC)
			omxDisplayFunction $1 $2
			;;
		AENC)
			omxDisplayFunction $1 $2
			;;
		VMFE)
			omxDisplayFunction $1 $2
		;;
		VVHE)
			omxDisplayFunction $1 $2
		;;
		mi)
			miDisplayFunciton all
		;;
		omx)
			omxDisplayFunction all
			;;
		isp)
			ispDisplayFunction
		;;
		ie)
			ieDisplayFunction
		;;
		all)
			echo ========================[PROC]MSTAR ALL MODULE START========================
			miDisplayFunciton all
			omxDisplayFunction all
			kernelDisplayFunction
			ispDisplayFunction
			ieDisplayFunction
			echo ========================[PROC]MSTAR ALL MODULE END========================
		;;
	*)
			echo "======error param======"
			helpFunction
		;;
	esac
}
main $1 $2