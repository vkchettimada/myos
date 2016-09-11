
KERNEL := myos
ARCH := ARCH_I386
MAPFILE := myos.map
export ARCH

LIBS := \
	arch/libarch.a \
	c/libc.a \
	m/libm.a \
	drivers/libdrivers.a \
	kernel/libkernel.a \
	apps/libapps.a

SRC := main.c
ASM := boot.S

VPATH := init

ifeq ("$(ARCH)", "ARCH_I386")
	VPATH += arch/i386/boot
endif


LOCAL_CFLAGS := -I./include -I./myosdefs
LOCAL_ASFLAGS := -I./include -I./myosdefs

#####################################################################
# Global variables
#####################################################################

SCRIPTS_DIR := $(PWD)/scripts
ECHO := echo
CAT := cat
COMPRESS := gzip
COMPRESS_OPTS := -v -f --best
COMPRESS_EXT := gz
CFLAGS := -Wall -Os -nostdinc -fno-builtin -D$(ARCH)
ASFLAGS := $(CFLAGS)
LDFLAGS := -nostdlib --cref -n -N
ifdef MAPFILE
	LDFLAGS += -Map $(MAPFILE)
endif
LDSCRIPT := $(SCRIPTS_DIR)/myos.ld
export SCRIPTS_DIR ECHO CAT CFLAGS ASFLAGS COMPRESS COMPRESS_OPTS \
	COMPRESS_EXT

#####################################################################
# Rules
#####################################################################
all: $(KERNEL)

$(KERNEL): $(LIBS) $(ASM:%.S=.obj/%.o) $(SRC:%.c=.obj/%.o)
	$(LD) -T $(LDSCRIPT) \
		$(LDFLAGS) \
		-o $@ $(ASM:%.S=.obj/%.o) $(SRC:%.c=.obj/%.o) \
		$(LIBS) $(LIBS)

.PHONY: compress

compress: $(KERNEL)
	$(COMPRESS) $(COMPRESS_OPTS) $(KERNEL)

include $(SCRIPTS_DIR)/make.rules

