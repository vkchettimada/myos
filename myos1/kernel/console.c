/* console.c */

#include "console.h"

static unsigned long vga_mem_base=CONSOLE_VGA_BASE;
static unsigned short *vga_pos=(unsigned short*)CONSOLE_VGA_BASE;
static unsigned char vga_attr=CONSOLE_VGA_ATTR;

void console_set_attr(unsigned char attr)
{
	vga_attr=attr;
}

int console_set_pos(int x,int y)
{
	if (x>=CONSOLE_COLS || y>=CONSOLE_ROWS)
		return -1;
	vga_pos=(unsigned short*)(vga_mem_base+(CONSOLE_COLS*y+x)*2);
	return 0;
}

int console_print(char *str)
{
	int len;

	for (len=0;str[len]!=0;len++)
	{
		*vga_pos=(unsigned short)((vga_attr<<8)|str[len]);
		vga_pos++;
	}
	return len;
}

