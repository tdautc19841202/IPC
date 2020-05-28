CROSS_COMPILE ?= arm-buildroot-linux-uclibcgnueabihf-
GCC ?= $(CROSS_COMPILE)gcc
GXX ?= $(CROSS_COMPILE)g++

APKAPI_TOP   = $(shell pwd)/../apkapi
MI_TOP       = $(shell pwd)/../../MI
# MD_TOP       = $(shell pwd)/../../IE/video/MTE/I3
ZLIB_TOP     = $(shell pwd)/../../zlib
OPENSSL_TOP  = $(shell pwd)/../../openssl
ZBAR_TOP     = $(shell pwd)/../../zbar
LIBMAD_TOP   = $(shell pwd)/../../libmad
LIBICONV_TOP = $(shell pwd)/../../libiconv
LIBRTSP_TOP  = $(shell pwd)/../librtsp
TUYA_TOP     = $(shell pwd)/../tuya
MIPACKAGE_TOP= $(shell pwd)/../MI_package

C_FLAGS := -Wall -Os -march=armv7-a -rdynamic -funwind-tables -ffunction-sections
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255

LDFLAGS := -ldl -lpthread -lm -lstdc++
LDFLAGS += -Wl,--start-group
LDFLAGS += -L$(MIPACKAGE_TOP)/lib -lmi_package
LDFLAGS += -L$(LIBRTSP_TOP)/lib -lrtsp
LDFLAGS += -L$(APKAPI_TOP)/lib -lapkapi
LDFLAGS += -L$(LIBMAD_TOP)/_install/lib -lmad
LDFLAGS += -L$(OPENSSL_TOP)/_install/lib -lcrypto -lssl
LDFLAGS += -L$(ZLIB_TOP)/_install/lib -lz
# LDFLAGS += -L$(MD_TOP)/lib -lMTE_LINUX
LDFLAGS += -L$(LIBICONV_TOP)/_install/lib -liconv
LDFLAGS += -L$(ZBAR_TOP)/_install/lib -lzbar
LDFLAGS += -L$(TUYA_TOP)/lib -ltuya_ipc -lmbedtls -lpthread -lrt -ldl -lm 
LDFLAGS += -Wl,--end-group

C_INCLUDES := -I$(MI_TOP)/include  \
			-I$(MIPACKAGE_TOP)/include \
			-I$(LIBRTSP_TOP) \
			-I$(APKAPI_TOP) \
			-I$(TUYA_TOP)/include
			# -I$(MD_TOP)/include  \

PROGS = ipcam

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o, $(OMX_COMP_C_SRCS_NO_DIR))

OMX_COMP_CPP_SRCS=$(wildcard ./*.cpp)
OMX_COMP_CPP_SRCS_NO_DIR=$(notdir $(OMX_COMP_CPP_SRCS))
CXXOBJS=$(patsubst %.cpp, %.cpp.o, $(OMX_COMP_CPP_SRCS_NO_DIR))

OBJDIR ?= $(shell pwd)/obj
BINDIR ?= $(shell pwd)/bin
INCDIR ?= $(shell pwd)/inc

OBJPROG = $(addprefix $(OBJDIR)/, $(PROGS))

.PHONY: clean prepare PROGS

all: prepare $(OBJPROG)

prepare:

clean:
	@rm -Rf $(OBJDIR)
	@rm -Rf $(BINDIR)

$(OBJPROG): $(addprefix $(OBJDIR)/, $(OBJECTS)) $(addprefix $(OBJDIR)/, $(CXXOBJS))
	@mkdir -p $(BINDIR)
	@echo "  BIN $@"
	$(GCC) -o $@ $(addprefix $(OBJDIR)/, $(OBJECTS)) $(addprefix $(OBJDIR)/, $(CXXOBJS)) $(LDFLAGS)
	@echo ""
	mv -f ${OBJDIR}/$(PROGS) ${BINDIR}
	
$(OBJDIR)/%.c.o : %.c
	@mkdir -p obj
	@echo "  CC  $<"
	$(GCC) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

$(OBJDIR)/%.cpp.o : %.cpp
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@
