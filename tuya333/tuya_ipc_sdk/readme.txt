[SDK files]
-include: TUYA IPC SDK API head files
-libs: TUYA IPC SDK libs
-demo_tuya_ipc: demo code to show how to use TUYA IPC SDK
	-user: 	demo source files
	-include: demo head files
	-resource: demo resources, video/audio/jpeg etc.
Note:
For QR code function, libz is suggested, which is a open source library.
Refer to https://superb-sea2.dl.sourceforge.net if needed.

[how to build a quick demo]
1. edit <Makefile>，set COMPILE_PREX ?= to absolute path of your toolchain gcc.
2. execute "sh build_app.sh demo_tuya_ipc" to build an application to run on board, locating at "output/demo_tuya_ipc/".