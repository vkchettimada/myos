/* stdio.h */
#ifndef _STDIO_H_
#define _STDIO_H_
extern void (*stdio_putchar)(int c);
void putchar (int c);
void printk (const char *format, ...);
#endif

