/* app2.c */
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

void app2_task(void)
{
	while(1) {	
		console_printf("app2_task: signaling\n");
		myOsSemSignal(&s);
		myOsSleep(1000);
	}
}

