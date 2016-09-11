/* app2.c */
#include <stdio.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>

void app2_task(void)
{
	while(1) {	
		printk("app2_task: waiting...\n");
		myos_wait(&s);
		printk("app2_task: signaled\n");		
	}
}

