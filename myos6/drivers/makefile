# drivers/makefile

SRC := \
	i8253.c \
	i8259a.c

SUBDIRS := audio char i2c pci video

VPATH := $(SUBDIRS)

include $(patsubst %,%/makefile,$(SUBDIRS))

CFLAGS += -I ../include -I ../myosdefs
 
include $(SCRIPTS_DIR)/make.rules

