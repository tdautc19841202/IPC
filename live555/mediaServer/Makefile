INCLUDES = -I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include
# Default library filename suffixes for each library that we link with.  The "config.*" file might redefine these later.
libliveMedia_LIB_SUFFIX = $(LIB_SUFFIX)
libBasicUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libgroupsock_LIB_SUFFIX = $(LIB_SUFFIX)
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

MEDIA_SERVER = live555MediaServer$(EXE)

PREFIX = 
ALL = $(MEDIA_SERVER)
all: $(ALL)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

MEDIA_SERVER_OBJS = live555MediaServer.$(OBJ) DynamicRTSPServer.$(OBJ)

live555MediaServer.$(CPP):	DynamicRTSPServer.hh version.hh
DynamicRTSPServer.$(CPP):	DynamicRTSPServer.hh

USAGE_ENVIRONMENT_DIR = ../UsageEnvironment
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.$(libUsageEnvironment_LIB_SUFFIX)
BASIC_USAGE_ENVIRONMENT_DIR = ../BasicUsageEnvironment
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/libBasicUsageEnvironment.$(libBasicUsageEnvironment_LIB_SUFFIX)
LIVEMEDIA_DIR = ../liveMedia
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/libliveMedia.$(libliveMedia_LIB_SUFFIX)
GROUPSOCK_DIR = ../groupsock
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/libgroupsock.$(libgroupsock_LIB_SUFFIX)
LOCAL_LIBS =	$(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
		$(BASIC_USAGE_ENVIRONMENT_LIB) $(USAGE_ENVIRONMENT_LIB)
LIBS =			$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION)

live555MediaServer$(EXE):	$(MEDIA_SERVER_OBJS) $(LOCAL_LIBS)
	$(LINK)$@ $(CONSOLE_LINK_OPTS) $(MEDIA_SERVER_OBJS) $(LIBS)

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

install: $(MEDIA_SERVER)
	  install -d $(DESTDIR)$(PREFIX)/bin
	  install -m 755 $(MEDIA_SERVER) $(DESTDIR)$(PREFIX)/bin

##### Any additional, platform-specific rules come here:
