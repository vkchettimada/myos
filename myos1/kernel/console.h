/* console.h */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#define CONSOLE_VGA_BASE 0xb8000L
#define CONSOLE_VGA_ATTR 0x07
#define CONSOLE_COLS 80
#define CONSOLE_ROWS 25

void console_set_attr(unsigned char attr);
int console_set_pos(int x,int y);
int console_print(char *str);

#endif

