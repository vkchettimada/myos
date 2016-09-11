/* console.h */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define CONSOLE_BUFFER_LEN (25*80*2)
#define CONSOLE_INPUT_BUFFER_LEN (256)
#define CONSOLE_MAX 1

typedef void (*console_ll_putchar_t)(char c);

typedef int (*console_handler_t)(char *);

extern console_ll_putchar_t console_ll_putchar;
void console_printf(char *fmt, ...);

#endif /* _CONSOLE_H_ */
