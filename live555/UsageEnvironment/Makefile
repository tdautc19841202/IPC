INCLUDES = -Iinclude -I../groupsock/include
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib
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

NAME = libUsageEnvironment
USAGE_ENVIRONMENT_LIB = $(NAME).$(LIB_SUFFIX)
ALL = $(USAGE_ENVIRONMENT_LIB)
all:	$(ALL)

OBJS = UsageEnvironment.$(OBJ) HashTable.$(OBJ) strDup.$(OBJ)

$(USAGE_ENVIRONMENT_LIB): $(OBJS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) $(OBJS)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

UsageEnvironment.$(CPP):	include/UsageEnvironment.hh
include/UsageEnvironment.hh:	include/UsageEnvironment_version.hh include/Boolean.hh include/strDup.hh
HashTable.$(CPP):		include/HashTable.hh
include/HashTable.hh:		include/Boolean.hh
strDup.$(CPP):			include/strDup.hh

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

install: install1 $(INSTALL2)
install1: $(USAGE_ENVIRONMENT_LIB)
	  install -d $(DESTDIR)$(PREFIX)/include/UsageEnvironment $(DESTDIR)$(LIBDIR)
	  install -m 644 include/*.hh $(DESTDIR)$(PREFIX)/include/UsageEnvironment
	  install -m 644 $(USAGE_ENVIRONMENT_LIB) $(DESTDIR)$(LIBDIR)
install_shared_libraries: $(USAGE_ENVIRONMENT_LIB)
	  ln -fs $(NAME).$(LIB_SUFFIX) $(DESTDIR)$(LIBDIR)/$(NAME).$(SHORT_LIB_SUFFIX)
	  ln -fs $(NAME).$(LIB_SUFFIX) $(DESTDIR)$(LIBDIR)/$(NAME).so

##### Any additional, platform-specific rules come here:
