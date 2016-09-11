# arch/makefile

ifeq ($(ARCH),ARCH_I386)
	VPATH := $(CURDIR)/i386
	include i386/makefile
endif

CFLAGS += -I ../include

include $(SCRIPTS_DIR)/make.rules

