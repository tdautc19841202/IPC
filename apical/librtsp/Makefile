GCC ?= $(CROSS_COMPILE)gcc
GXX ?= $(CROSS_COMPILE)g++

LIVE555_TOP = $(shell pwd)/../../live555

C_FLAGS := -Wall -Os -march=armv7-a
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255

C_INCLUDES := \
	-I$(LIVE555_TOP)/_install/usr/local/include/liveMedia \
	-I$(LIVE555_TOP)/_install/usr/local/include/groupsock \
	-I$(LIVE555_TOP)/_install/usr/local/include/UsageEnvironment \
	-I$(LIVE555_TOP)/_install/usr/local/include/BasicUsageEnvironment

PROGS = librtsp.a

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o, $(OMX_COMP_C_SRCS_NO_DIR))

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
	$(AR) x $(LIVE555_TOP)/_install/usr/local/lib/libliveMedia.a
	$(AR) x $(LIVE555_TOP)/_install/usr/local/lib/libgroupsock.a
	$(AR) x $(LIVE555_TOP)/_install/usr/local/lib/libBasicUsageEnvironment.a
	$(AR) x $(LIVE555_TOP)/_install/usr/local/lib/libUsageEnvironment.a
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
	@$(GXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

