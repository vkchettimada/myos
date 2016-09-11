
OBJS= boot.o main.o stdio.o stdlib.o vga.o pci.o io.o string.o
CFLAGS= -Os -nostdinc -fno-builtin
LDFLAGS= -nostdlib

all: myos1 myos2 myos3

myos1: $(OBJS)
	ld -T myos1.ld $(LDFLAGS) -o $@ $(OBJS)

myos2: $(OBJS)
	ld -T myos2.ld $(LDFLAGS) -o $@ $(OBJS)

myos3: $(OBJS)
	ld $(LDFLAGS) -o $@ $(OBJS)

*.o: *.c *.S
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f myos1 myos2 myos3 *.o

