# c/makefile

SRC := \
	i8253.c \
	i8259a.c

SUBDIRS := char pci video

VPATH := $(SUBDIRS)

include $(patsubst %,%/makefile,$(SUBDIRS))

CFLAGS += -I ../include -I ../myosdefs
 
%.a: $(SRC:.c=.o)
	$(AR) r $@ $(SRC:.c=.o)

clean:
	rm -f *.a $(SRC:.c=.o)

