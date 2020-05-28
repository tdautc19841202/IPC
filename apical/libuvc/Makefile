GCC ?= $(CROSS_COMPILE)gcc

# MI_TOP  = $(shell pwd)/../../MI

C_FLAGS := -Wall -Os -march=armv7-a
C_FLAGS += -fPIC -DPIC -DOMXILCOMPONENTSPATH=\"/$(BUILD_DIR)\" -DCONFIG_DEBUG_LEVEL=255

C_INCLUDES := \
    -I$(shell pwd)/include
    # -I$(MI_TOP)/include \

PROGS = libuvc.a

OMX_COMP_C_SRCS=$(wildcard ./*.c)
OMX_COMP_C_SRCS_NO_DIR=$(notdir $(OMX_COMP_C_SRCS))
OBJECTS=$(patsubst %.c, %.c.o, $(OMX_COMP_C_SRCS_NO_DIR))

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
	@$(AR) rcs $@ $(OBJDIR)/*.o
	@echo ""
	mv -f $(OBJDIR)/$(PROGS) $(LIBDIR)
	
$(OBJDIR)/%.c.o : %.c
	@mkdir -p obj
	@echo "  CC  $<"
	@$(GCC) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

