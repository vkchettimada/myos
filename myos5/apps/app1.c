/* app1.c */
#include <stdio.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>

void app1_task(void)
{
	char c;

	while(1) {
		while (myOsQueueRemove(&queue_kbd,&c))
			myOsSleep(0);
		putchar(c);
	}
}

