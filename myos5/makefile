KERNEL := myos
ARCH := ARCH_I386
MAPFILE := myos.map
export ARCH

LIBS := arch c drivers kernel apps
SRC := init/main.c
ASM := 

ifeq ($(ARCH),ARCH_I386)
	ASM += arch/i386/boot/boot.S
endif

ABSLIBS := $(foreach dir,$(LIBS),$(dir)/lib$(dir).a)

CFLAGS := -Wall -Os -nostdinc -fno-builtin -D$(ARCH)
ASFLAGS := $(CFLAGS)
export CFLAGS ASFLAGS
LDFLAGS := -nostdlib --cref
ifdef MAPFILE
	LDFLAGS += -Map $(MAPFILE)
endif
LDSCRIPT := scripts/myos1.ld

all: $(KERNEL)

$(KERNEL): $(ABSLIBS) $(ASM:.S=.o) $(SRC:.c=.o)
#	$(LD) -T $(LDSCRIPT) \
#		$(LDFLAGS) \
#		$(foreach lib,$(dir $(ABSLIBS)),-L$(lib)) \
#		-o $@ $(ASM:.S=.o) $(SRC:.c=.o) \
#		$(foreach lib,$(LIBS),-l$(lib))
#
	$(LD) -T $(LDSCRIPT) \
		$(LDFLAGS) \
		-o $@ $(ASM:.S=.o) $(SRC:.c=.o) \
		$(ABSLIBS) $(ABSLIBS)

$(ABSLIBS):
	$(MAKE) -C $(@D) $(@F)

$(ASM:.S=.o): ASFLAGS += -I./include -I./myosdefs

$(SRC:.c=.o): CFLAGS += -I./include -I./myosdefs
	
.PHONY: release debug perfect clean

release: all

release: LDFLAGS += -s

debug: all
	
debug: CFLAGS += -g

perfect: all

perfect: CFLAGS += -Werror
	
clean:
	for item in $(LIBS); \
	do \
		$(MAKE) -C $$item clean; \
	done
	$(RM) -f $(ASM:.S=.o) $(SRC:.c=.o) $(KERNEL) $(MAPFILE)
       	
