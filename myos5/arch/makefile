# arch/makefile

ifeq ($(ARCH),ARCH_I386)
	VPATH := $(CURDIR)/i386
	include i386/makefile
endif

CFLAGS += -I ../include

%.a: $(ASM:.S=.o) $(SRC:.c=.o)
	$(AR) r $@ $(ASM:.S=.o) $(SRC:.c=.o)

clean:
	rm -f *.a $(ASM:.S=.o) $(SRC:.c=.o)

