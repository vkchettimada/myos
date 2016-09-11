# c/makefile

SRC := \
	stdio.c \
	stdlib.c \
	string.c

CFLAGS += -I ../include

%.a: $(SRC:.c=.o)
	$(AR) r $@ $(SRC:.c=.o)

clean:
	rm -f *.a $(SRC:.c=.o)

