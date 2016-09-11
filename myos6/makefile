
KERNEL := myos
ARCH := ARCH_I386
MAPFILE := myos.map
export ARCH

LIBS := \
	arch/libarch.a \
	c/libc.a \
	drivers/libdrivers.a \
	kernel/libkernel.a \
	apps/libapps.a
SRC := init/main.c
ASM := 

ifeq ($(ARCH),ARCH_I386)
	ASM += arch/i386/boot/boot.S
endif


LOCAL_CFLAGS := -I./include -I./myosdefs
LOCAL_ASFLAGS := -I./include -I./myosdefs

#####################################################################
# Global variables
#####################################################################

SCRIPTS_DIR := $(PWD)/scripts
ECHO :=echo
CAT :=cat
CFLAGS := -Wall -Os -nostdinc -fno-builtin -D$(ARCH)
ASFLAGS := $(CFLAGS)
LDFLAGS := -nostdlib --cref
ifdef MAPFILE
	LDFLAGS += -Map $(MAPFILE)
endif
LDSCRIPT := $(SCRIPTS_DIR)/myos1.ld
export SCRIPTS_DIR ECHO CAT CFLAGS ASFLAGS

#####################################################################
# Rules
#####################################################################
all: $(KERNEL)

$(KERNEL): $(LIBS) $(ASM:.S=.o) $(SRC:.c=.o)
	$(LD) -T $(LDSCRIPT) \
		$(LDFLAGS) \
		-o $@ $(ASM:.S=.o) $(SRC:.c=.o) \
		$(LIBS) $(LIBS)

include $(SCRIPTS_DIR)/make.rules

