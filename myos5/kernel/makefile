# c/makefile

SRC := \
	myos.c \
	myos_irq.c \
	myos_sem.c \
	myos_queue.c \
	myos_debug.c \
	myos_globals.c

CFLAGS += -I ../include -I ../myosdefs

%.a: $(SRC:.c=.o)
	$(AR) r $@ $(SRC:.c=.o)

clean:
	rm -f *.a $(SRC:.c=.o)

