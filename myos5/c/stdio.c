/* stdio.c */
#include "stdio.h"
#include "stdlib.h"

void (*stdio_putchar)(int c);

/* Put the character C on the screen. */
void putchar (int c)
{
	if (stdio_putchar)
		stdio_putchar(c);
}

/* Format a string and print it on the screen, just like the libc
 *         function printf. */
void printk (const char *format, ...)
{
 	char **arg = (char **) &format;
 	int c;
 	char buf[20];
   	
 	arg++;
   	
 	while ((c = *format++) != 0) {
		if (c != '%')
			putchar (c);
     		else {
	 		char *p;
		   	
	 		c = *format++;
	 		switch (c) {
	       			case 'd':
	       			case 'u':
	       			case 'x':
		     			itoa (buf, c, *((int *) arg++));
		     			p = buf;
		     			goto string;
		     			break;
				case 's':
		     			p = *arg++;
		     			if (! p)
			   			p = "(null)";
string:
		     			while (*p)
			   			putchar (*p++);
		     			break;				   	
	       			default:
		     			putchar (*((int *) arg++));
		     			break;
       			}
   		}
	}
}

