# c/makefile

SRC := \
	app1.c \
	app2.c \
	app3.c \
	app4.c

CFLAGS += -I ../include -I ../myosdefs

%.a: $(SRC:.c=.o)
	$(AR) r $@ $(SRC:.c=.o)

clean:
	rm -f *.a $(SRC:.c=.o)

