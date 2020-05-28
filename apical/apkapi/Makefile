GCC ?= $(CROSS_COMPILE)gcc
AR  ?= $(CROSS_COMPILE)ar
GXX ?= $(CROSS_COMPILE)g++

DRIVER_HEADER= $(shell pwd)/../../kernel/drivers/sstar/include
MI_TOP       = $(shell pwd)/../../MI
# MD_TOP       = $(shell pwd)/../../IE/video/MTE/I3
CURL_TOP     = $(shell pwd)/../../curl
ZBAR_TOP     = $(shell pwd)/../../zbar
SQLITE_TOP   = $(shell pwd)/../../sqlite
LIBRTMP_TOP  = $(shell pwd)/../../librtmp
LIBMAD_TOP   = $(shell pwd)/../../libmad
LIBFAAC_TOP  = $(shell pwd)/../../faac
LIBMTD_TOP   = $(shell pwd)/../../mtd-utils
LIBAVI_TOP   = $(shell pwd)/../libavi
LIBMP4_TOP   = $(shell pwd)/../libmp4
MIPACKAGE_TOP= $(shell pwd)/../MI_package

C_FLAGS := -Wall -Os -march=armv7-a
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255

C_INCLUDES := \
	-I$(DRIVER_HEADER) \
	-I$(MI_TOP)/include \
	-I$(MIPACKAGE_TOP)/include \
	-I$(CURL_TOP)/_install/include \
	-I$(ZBAR_TOP)/_install/include \
	-I$(SQLITE_TOP)/_install/include \
	-I$(LIBRTMP_TOP)/_install/include \
	-I$(LIBMAD_TOP)/_install/include \
	-I$(LIBFAAC_TOP)/_install/include \
	-I$(LIBMTD_TOP)/include \
	-I$(LIBAVI_TOP) \
	-I$(LIBMP4_TOP)
	# -I$(MD_TOP)/include \

PROGS = libapkapi.a

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o,  $(OMX_COMP_C_SRCS_NO_DIR))

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

$(OBJPROG): $(addprefix $(OBJDIR)/, $(OBJECTS))
	@mkdir -p $(LIBDIR)
	@echo "  LIBDIR $@"
	$(AR) x $(LIBMTD_TOP)/arm-buildroot-linux-uclibcgnueabihf/lib/libmtd.a
	mv -f *.o $(OBJDIR)
	@$(AR) rcs $@ $(OBJDIR)/*.o
	@echo ""
	mv -f ${OBJDIR}/$(PROGS) ${LIBDIR}

$(OBJDIR)/%.c.o : %.c
	@mkdir -p obj
	@echo "  CC  $<"
	$(GCC) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

$(OBJDIR)/%.cpp.o : %.cpp
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

