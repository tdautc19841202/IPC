GCC ?= $(CROSS_COMPILE)gcc
AR  ?= $(CROSS_COMPILE)ar
GXX ?= $(CROSS_COMPILE)g++

DRIVER_HEADER= $(shell pwd)/../../kernel/drivers/sstar/include
MI_TOP       = $(shell pwd)/../../MI
LIVE555_TOP  = $(shell pwd)/../../live555
APKAPI_TOP   = $(shell pwd)/../apkapi

C_FLAGS := -Wall -Os -march=armv7-a
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255

C_INCLUDES := \
	-I$(DRIVER_HEADER) \
	-I$(MI_TOP)/include \
	-I$(shell pwd)/include \
	-I$(APKAPI_TOP) \
	-I$(LIVE555_TOP)/BasicUsageEnvironment/include \
	-I$(LIVE555_TOP)/liveMedia/include \
	-I$(LIVE555_TOP)/mediaServer/include \
	-I$(LIVE555_TOP)/UsageEnvironment/include \
	-I$(LIVE555_TOP)/groupsock/include

PROGS = libmi_package.a

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o,  $(OMX_COMP_C_SRCS_NO_DIR))

OMX_COMP_CPP_SRCS=$(wildcard ./*.cpp)
OMX_COMP_CPP_SRCS_NO_DIR=$(notdir $(OMX_COMP_CPP_SRCS))
CXXOBJS=$(patsubst %.cpp, %.cpp.o, $(OMX_COMP_CPP_SRCS_NO_DIR))

OBJDIR ?= $(shell pwd)/obj
LIBDIR ?= $(shell pwd)/lib
INCDIR ?= $(shell pwd)/inc

OBJPROG = $(addprefix $(OBJDIR)/, $(PROGS))

.PHONY: clean prepare PROGS

all: prepare $(OBJPROG)

prepare:

clean:
	@rm -Rf $(OBJDIR)
	@rm -Rf $(LIBDIR)

$(OBJPROG): $(addprefix $(OBJDIR)/, $(OBJECTS)) $(addprefix $(OBJDIR)/, $(CXXOBJS))
	@mkdir -p $(LIBDIR)
	@echo "  LIBDIR $@"
	$(AR) x $(MI_TOP)/lib/static/libmi_sys.a
	$(AR) x $(MI_TOP)/lib/static/libmi_common.a
	$(AR) x $(MI_TOP)/lib/static/libmi_vpe.a
	$(AR) x $(MI_TOP)/lib/static/libmi_isp.a
	$(AR) x $(MI_TOP)/lib/static/libmi_common.a
	$(AR) x $(MI_TOP)/lib/static/libmi_sensor.a
	$(AR) x $(MI_TOP)/lib/static/libmi_venc.a
	$(AR) x $(MI_TOP)/lib/static/libmi_vif.a
	$(AR) x $(MI_TOP)/lib/static/libmi_rgn.a
	$(AR) x $(MI_TOP)/lib/static/libmi_divp.a
	$(AR) x $(MI_TOP)/lib/static/libmi_iqserver.a
	$(AR) x $(MI_TOP)/lib/static/libmi_vdf.a
	$(AR) x $(MI_TOP)/lib/static/libmi_shadow.a
	$(AR) x $(MI_TOP)/lib/static/libOD_LINUX.a
	$(AR) x $(MI_TOP)/lib/static/libMD_LINUX.a
	$(AR) x $(MI_TOP)/lib/static/libVG_LINUX.a
	$(AR) x $(MI_TOP)/lib/static/libmi_ive.a
	$(AR) x $(MI_TOP)/lib/static/libmi_ai.a
	$(AR) x $(MI_TOP)/lib/static/libmi_ao.a
	$(AR) x $(MI_TOP)/lib/static/libcus3a.a
	$(AR) x $(MI_TOP)/lib/static/libispalgo.a
	$(AR) x $(LIVE555_TOP)/_install/usr/local/lib/libliveMedia.a
	mv -f *.o $(OBJDIR)
	@$(AR) rcs $@ $(OBJDIR)/*.o
	@echo ""
	mv -f $(OBJDIR)/$(PROGS) $(LIBDIR)

$(OBJDIR)/%.c.o : %.c
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GCC) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

$(OBJDIR)/%.cpp.o : %.cpp
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GXX) $(C_FLAGS) -std=c++11 $(C_INCLUDES) -c $< -o $@

