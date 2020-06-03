CROSS_COMPILE ?= arm-linux-gnueabihf-
GCC   ?= $(CROSS_COMPILE)gcc
STRIP ?= $(CROSS_COMPILE)strip

C_FLAGS := -Wall -Os -march=armv7-a
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

PROGS = fsckfat

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o, $(OMX_COMP_C_SRCS_NO_DIR))

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

$(OBJPROG): $(addprefix $(OBJDIR)/, $(OBJECTS))
	@mkdir -p $(BINDIR)
	@echo "  BIN $@"
	@$(GCC) -o $@ $(addprefix $(OBJDIR)/, $(OBJECTS)) $(LDFLAGS)
	@$(STRIP) $@
	@echo ""
	mv -f ${OBJDIR}/$(PROGS) ${BINDIR}

$(OBJDIR)/%.c.o : %.c
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GCC) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@
