TARGET_DEVICE=/dev/fd0
CC=gcc
LD=ld
LDFLAGS= -e my_start -Tdata 0x10ff0 -Ttext 0x10000 -s --oformat binary
CFLAGS=

MYOS_OBJ = main.o console.o

all: myos

myos: $(MYOS_OBJ)
	$(LD) $(LDFLAGS) -o $@ $(MYOS_OBJ)

*.o:

clean:
	rm -rf *.o myos

install: myos
	dd if=$< of=$(TARGET_DEVICE) bs=512 seek=1


