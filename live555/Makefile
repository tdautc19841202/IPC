##### Change the following for your environment:
CROSS_COMPILE=         arm-buildroot-linux-uclibcgnueabihf-
COMPILE_OPTS =         $(INCLUDES) -I. -Os -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -DLARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DALLOW_RTSP_SERVER_PORT_REUSE -DBSD=1 -DLOCALE_NOT_USED -DSO_REUSEPORT
C =                    c
C_COMPILER =           $(CROSS_COMPILE)gcc
C_FLAGS =              $(COMPILE_OPTS)
CPP =                  cpp
CPLUSPLUS_COMPILER =   $(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =      $(COMPILE_OPTS) -Wall -DBSD=1
OBJ =                  o
LINK =                 $(CROSS_COMPILE)gcc -o
LINK_OPTS =            -L. -lstdc++
CONSOLE_LINK_OPTS =    $(LINK_OPTS)
LIBRARY_LINK =         $(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =    
LIB_SUFFIX =           a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
INC += $(CUR_DIR)/BasicUsageEnvironment/include
INC += $(CUR_DIR)/groupsock/include
INC += $(CUR_DIR)/liveMedia/include
INC += $(CUR_DIR)/UsageEnvironment/include
INC += $(CUR_DIR)/mediaServer/include

SUBDIRS += $(CUR_DIR)/BasicUsageEnvironment
SUBDIRS += $(CUR_DIR)/groupsock
SUBDIRS += $(CUR_DIR)/liveMedia
SUBDIRS += $(CUR_DIR)/UsageEnvironment
SUBDIRS += $(CUR_DIR)/mediaServer

##### End of variables to change

LIVEMEDIA_DIR = liveMedia
GROUPSOCK_DIR = groupsock
USAGE_ENVIRONMENT_DIR = UsageEnvironment
BASIC_USAGE_ENVIRONMENT_DIR = BasicUsageEnvironment

TESTPROGS_DIR = testProgs

MEDIA_SERVER_DIR = mediaServer

PROXY_SERVER_DIR = proxyServer

all:
	cd $(LIVEMEDIA_DIR) ; $(MAKE)
	cd $(GROUPSOCK_DIR) ; $(MAKE)
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE)
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE)
	cd $(TESTPROGS_DIR) ; $(MAKE)
	cd $(MEDIA_SERVER_DIR) ; $(MAKE)
	cd $(PROXY_SERVER_DIR) ; $(MAKE)
	@echo
	@echo "For more information about this source code (including your obligations under the LGPL), please see our FAQ at http://live555.com/liveMedia/faq.html"

install:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) install
	cd $(GROUPSOCK_DIR) ; $(MAKE) install
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) install
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) install
	cd $(TESTPROGS_DIR) ; $(MAKE) install
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) install
	cd $(PROXY_SERVER_DIR) ; $(MAKE) install

clean:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) clean
	cd $(GROUPSOCK_DIR) ; $(MAKE) clean
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(TESTPROGS_DIR) ; $(MAKE) clean
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) clean
	cd $(PROXY_SERVER_DIR) ; $(MAKE) clean

distclean: clean
	-rm -f $(LIVEMEDIA_DIR)/Makefile $(GROUPSOCK_DIR)/Makefile \
	  $(USAGE_ENVIRONMENT_DIR)/Makefile $(BASIC_USAGE_ENVIRONMENT_DIR)/Makefile \
	  $(TESTPROGS_DIR)/Makefile $(MEDIA_SERVER_DIR)/Makefile \
	  $(PROXY_SERVER_DIR)/Makefile Makefile
