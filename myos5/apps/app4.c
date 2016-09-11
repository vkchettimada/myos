/* app4.c */
#include <stdio.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>

void app4_task(void)
{
	while(1) {	
		printk("app4_task: waiting...\n");
		myos_wait(&s);
		printk("app4_task: signaled\n");		
	}
}

