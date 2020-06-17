#!/bin/sh

APP_BIN_NAME=
if [ -z "$1" ];then
	echo "please input the target app name "
	exit 1
else
	APP_BIN_NAME=$1
fi

#开始编译APP
if [ -z "$2" ];then
    make APP_BIN_NAME=$APP_BIN_NAME build_app
else
    make APP_BIN_NAME=$APP_BIN_NAME COMPILE_PREX=$2 build_app
fi
