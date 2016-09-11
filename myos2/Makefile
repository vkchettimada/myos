
OBJS= boot.o main.o
CFLAGS= -nostdinc -fno-builtin

all: myos

myos: $(OBJS)
	ld -T myos.ld -o $@ $(OBJS)

*.o: *.c *.S
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f myos *.o

