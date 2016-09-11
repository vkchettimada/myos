/* app3.c */
#include <stdio.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>

void app3_task(void)
{
	while(1) {	
		printk("app3_task: signaling\n");
		myos_signal(&s);
		myOsSleep(1);
	}
}

