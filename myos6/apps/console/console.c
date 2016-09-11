/* console.c */
#include <stdio.h>
#include <stdlib.h>
#include <asm/delay.h>
#include <myos/myos.h>
#include <myos/myos_globals.h>
#include <apps/console/console.h>

console_ll_putchar_t console_ll_putchar = 0;

typedef struct
{
	char buffer[CONSOLE_BUFFER_LEN];
	int buffer_done_index;
	int buffer_todo_index;
	int flags;
#define CONSOLE_FLAG_ENABLE  0x00000001
#define CONSOLE_FLAG_PROCESS 0x00000002
#define CONSOLE_FLAG_ECHO    0x00000004
#define CONSOLE_FLAG_STRING  0x00000008
	char input_buffer[CONSOLE_INPUT_BUFFER_LEN];
	int input_buffer_len;
	console_handler_t handler;
} console_t;

#define CONSOLE_IS_FLAG(con, flag) (con->flags&flag)
#define CONSOLE_SET_FLAG(con, flag) con->flags |= flag
#define CONSOLE_RESET_FLAG(con, flag) con->flags &= (~flag)

static int console_curr=0;
static console_t console[CONSOLE_MAX];

static int console_default_handler(char *s);

void console_task(void)
{
	char c;
	int result;

	console[console_curr].handler = console_default_handler;

	while(1) {
		console_t *con = &console[console_curr];
	
		while ( (result = myOsQueueRemove(&queue_kbd,&c))
				&& !CONSOLE_IS_FLAG(con, CONSOLE_FLAG_PROCESS) ) {
			myOsSleep(0);
		}
		if (result == 0) {
			if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_ECHO) && console_ll_putchar )
				console_ll_putchar(c);

			if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_STRING) 
					|| (con->handler == console_default_handler) ) {
				if ( (con->input_buffer_len < (CONSOLE_INPUT_BUFFER_LEN-1))
						&& (c != '\n') ) {
					con->input_buffer[con->input_buffer_len++]=c;
				} else {
					con->input_buffer[con->input_buffer_len]='\0';
					con->handler(con->input_buffer);
					con->input_buffer_len=0;
				}
			} else {
				con->handler(&c);
			}
		}
		if ( CONSOLE_IS_FLAG(con, CONSOLE_FLAG_PROCESS) && console_ll_putchar ) {
			int next;
			
			while ( (next=(con->buffer_done_index+1)%CONSOLE_BUFFER_LEN) != con->buffer_todo_index ) {
				console_ll_putchar(con->buffer[con->buffer_done_index]);
				con->buffer_done_index = next;
			}
			CONSOLE_RESET_FLAG(con, CONSOLE_FLAG_PROCESS);
		}
	}
}

static int console_default_handler(char *s)
{
	printk("%s\n",s);
	return(0);
}

char console_putchar(char c)
{
	console_t *con = &console[console_curr];

	con->buffer_todo_index = (con->buffer_todo_index + 1) % CONSOLE_BUFFER_LEN;
	if (con->buffer_todo_index == con->buffer_done_index)
		con->buffer_done_index = (con->buffer_done_index + 1) % CONSOLE_BUFFER_LEN;
	con->buffer[con->buffer_todo_index]=c;
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

