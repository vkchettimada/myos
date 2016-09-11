/* console.h */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define CONSOLE_BUFFER_LEN (25*80*2)
#define CONSOLE_INPUT_BUFFER_LEN (256)
#define CONSOLE_MAX 10

#define CONSOLE_FLAG_ENABLE  0x00000001
#define CONSOLE_FLAG_PROCESS 0x00000002
#define CONSOLE_FLAG_ECHO    0x00000004
#define CONSOLE_FLAG_STRING  0x00000008

typedef void (*console_ll_putchar_t)(char c);

typedef int (*console_handler_t)(char *);

extern console_ll_putchar_t console_ll_putchar;

int console_create(char *key, int flags, console_handler_t handler);
void console_printf(int log, char *fmt, ...);

#endif /* _CONSOLE_H_ */

