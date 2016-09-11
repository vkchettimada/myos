
OBJS := arch/i386/boot/boot.o \
		init/main.o
ARCH_OBJS := arch/i386/mem.o \
		arch/i386/io.o \
		arch/i386/entry.o \
		arch/i386/i8259a.o \
		arch/i386/irq.o \
		arch/i386/i8253.o \
		arch/i386/tasks.o
LIB_OBJS := lib/stdio.o \
		lib/stdlib.o \
		lib/string.o
DRV_OBJS := drivers/video/vga.o \
		drivers/pci/pci.o \
		drivers/input/kbd.o
MYOS_OBJS := kernel/myos.o \
		kernel/myos_sem.o

# CFLAGS := -DMYOS_DEBUG

CFLAGS := $(CFLAGS) -Os -nostdinc -fno-builtin -I./include
LDFLAGS := -nostdlib

SCRIPT1 := scripts/myos1.ld
SCRIPT2 := scripts/myos2.ld

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

all: myos1 myos2 myos3

myos1: $(OBJS) $(MYOS_OBJS) $(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)
	ld -T $(SCRIPT1) $(LDFLAGS) -o $@ $(OBJS) $(MYOS_OBJS) \
		$(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)

myos2: $(OBJS) $(MYOS_OBJS) $(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)
	ld -T $(SCRIPT2) $(LDFLAGS) -o $@ $(OBJS) $(MYOS_OBJS) \
		$(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)

myos3: $(OBJS) $(MYOS_OBJS) $(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)
	ld $(LDFLAGS) -o $@ $(OBJS) $(MYOS_OBJS) $(ARCH_OBJS) \
		$(LIB_OBJS) $(DRV_OBJS)

clean:
	rm -f myos1 myos2 myos3 $(OBJS) $(MYOS_OBJS) $(ARCH_OBJS) $(LIB_OBJS) $(DRV_OBJS)

