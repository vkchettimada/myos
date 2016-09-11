/* app3.c */
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

void app3_task(void)
{
	while(1) {	
		console_printf(1, "app3_task: waiting...\n");
		myOsSemWait(&s);
		console_printf(1, "app3_task: signaled\n");		
	}
}

