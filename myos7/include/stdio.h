/* stdio.h */
#ifndef _STDIO_H_
#define _STDIO_H_
extern void (*stdio_putchar)(char c);
void putchar(char c);
void printk(const char *format, ...);
#endif

