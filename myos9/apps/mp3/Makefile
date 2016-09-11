
CC=gcc
# CFLAGS=-Wall -O2 -m486 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math
CFLAGS=-Wall -O2 -march=i686 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math
LDFLAGS= -lm

MPG_OBJ = common.o tables.o huffman.o layer1.o layer2.o layer3.o synth.o
MPGLIB_OBJ = $(MPG_OBJ) mpglib.o
MAKE_OUT = mpglib.a mp12

all: $(MAKE_OUT)

*.o: common.h mpega.h huffman.h mpglib.h

mpglib.a: $(MPGLIB_OBJ)
	ar -r $@ $(MPGLIB_OBJ)
	
mp12: mpglib.a mp12.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ mp12.o mpglib.a

clean:
	rm *.o $(MAKE_OUT) 
