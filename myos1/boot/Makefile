AS=as
LD=ld
TARGET_DEVICE=/dev/fd0

all: boot	

boot: boot.o
	$(LD) -o $@ -s -Ttext 0x0 --oformat binary $<

boot.o: boot.S
	$(AS) -o $@ $<

clean: 
	rm -rf boot *.o 

install: boot
	dd if=$< of=$(TARGET_DEVICE)

