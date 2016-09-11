/* console.c */
#include <myos/myos.h>
#include <myos/myos_globals.h>

#include <console_cfg.h>


typedef struct
{
	char buffer[CONSOLE_BUFFER_LEN];
	int buffer_done_index;
	int buffer_todo_index;
} console_t;

static int console_curr=0;
static console_t console[CONSOLE_MAX];

void console_task(void)
{
	char c;
	int result;

	while(1) {
		while ( (result = myOsQueueRemove(&queue_kbd,&c))
				|| myOsSemPending(console_sem) )
			myOsSleep(0);
		if (result == 0) {
			putchar(c);
		} else {
			
		}
	}
}

char console_putchar(char c)
{
	console_t *con = &console[console_curr];

	con->buffer[buffer_todo_index++]=c;
}

char *console_printf(char *fmt, ...)
{
}

