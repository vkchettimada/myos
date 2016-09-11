/* console.c */
#include <stdlib.h>
#include <string.h>
#include <asm/delay.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

console_ll_putchar_t console_ll_putchar = 0;

typedef struct
{
	char *key;
	char buffer[CONSOLE_BUFFER_LEN];
	int buffer_done_index;
	int buffer_todo_index;
	int flags;
	char input_buffer[CONSOLE_INPUT_BUFFER_LEN];
	int input_buffer_len;
	console_handler_t handler;
} console_t;

#define CONSOLE_IS_FLAG(con, flag) (con->flags&flag)
#define CONSOLE_SET_FLAG(con, flag) con->flags |= flag
#define CONSOLE_RESET_FLAG(con, flag) con->flags &= (~flag)

static int console_curr=0, console_count;
static console_t console[CONSOLE_MAX];

static int console_default_handler(char *s);

void console_task(void)
{
	char c, prev_c=0;
	int result, con_res;

	console_count=1;
	console[console_curr].handler = console_default_handler;
	console[console_curr].flags = 0x5;
	console[console_curr].key = "root";

	while(1) {
		console_t *con = &console[console_curr];
	
		while ( (result = myOsQueueRemove(&queue_kbd,&c))
				&& !CONSOLE_IS_FLAG(con, CONSOLE_FLAG_PROCESS) ) {
			myOsSleep(0);
		}
		if (result == 0) {
			if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_ECHO) && console_ll_putchar )
				console_ll_putchar(c);

			con_res = 0;
			if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_STRING) 
					|| (con->handler == console_default_handler) ) {
				if ( (con->input_buffer_len < (CONSOLE_INPUT_BUFFER_LEN-1))
						&& (c != '\n') ) {
					con->input_buffer[con->input_buffer_len++]=c;
				} else {
					con->input_buffer[con->input_buffer_len]='\0';
					con_res = con->handler(con->input_buffer);
					con->input_buffer_len=0;
				}
			} else {
				con_res = con->handler(&c);
			}

			if (c == '\n') {
				if (con_res || prev_c == '/') {
					console_curr = 0;
				}
				con = &console[console_curr];
				console_printf("%s>", con->key);
				CONSOLE_SET_FLAG(con, CONSOLE_FLAG_PROCESS);
			}
			
			prev_c = c;
		}
		if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_PROCESS) && console_ll_putchar ) {
			while ( con->buffer_done_index != con->buffer_todo_index ) {
				console_ll_putchar(con->buffer[con->buffer_done_index]);
				con->buffer_done_index = (con->buffer_done_index+1)%CONSOLE_BUFFER_LEN;
			}
			CONSOLE_RESET_FLAG(con, CONSOLE_FLAG_PROCESS);
		}
	}
}

static int console_default_handler(char *s)
{
	int console_index;

	if (*s != '\\' || *s=='\0')
		return(0);

	s++;
	for (console_index = 0; console_index < console_count; console_index++) {
		if (!strcmp(s, console[console_index].key)) {
			console_curr = console_index;
		}
	}
	
	return(0);
}

int console_create(char *key, int flags, console_handler_t handler)
{
	int console_id=-1;

	if (console_count >= CONSOLE_MAX) {
		console_printf("console: max consoles registered\n");
		return(console_id);
	}

	console_id = console_count++;
	console[console_id].key = key;
	console[console_id].flags = flags;
	console[console_id].handler = handler;
	
	return(console_id);
}
		
char console_putchar(char c)
{
	console_t *con = &console[console_curr];

	con->buffer[con->buffer_todo_index]=c;
	con->buffer_todo_index = (con->buffer_todo_index + 1) % CONSOLE_BUFFER_LEN;
	if (con->buffer_todo_index == con->buffer_done_index)
		con->buffer_done_index = (con->buffer_done_index + 1) % CONSOLE_BUFFER_LEN;
	if (c == '\n')
		CONSOLE_SET_FLAG(con, CONSOLE_FLAG_PROCESS);
	return(c);
}

static void console_puts(char *s)
{
	while(*s)
		console_putchar(*s++);
}

void console_printf(char *fmt, ...)
{
 	char **arg = (char **) &fmt;
 	int c;
 	char buf[20];
   	
	/* prefix the time */
	console_puts("::");
	itoa(buf,'d',timer_ticks);
	console_puts(buf);
	console_puts("::");
	
	arg++;
   	
 	while ((c = *fmt++) != 0) {
		if (c != '%')
			console_putchar(c);
     		else {
	 		c = *fmt++;
	 		switch (c) {
	       			case 'd':
	       			case 'u':
	       			case 'x':
		     			itoa(buf, c, *((int *) arg++));
					console_puts(buf);
		     			break;
				case 's':
					{
						char *p;
						p = *arg++;
						if (! p)
							p = "(null)";
						console_puts(p);
					}
		     			break;				   	
	       			default:
		     			console_putchar(*((int *) arg++));
		     			break;
       			}
   		}
	}
}

