/* app1.c */
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

void app1_task(void)
{
	while(1) {	
		console_printf("app1_task: waiting...\n");
		myOsSemWait(&s);
		console_printf("app1_task: signaled\n");
	}
}

