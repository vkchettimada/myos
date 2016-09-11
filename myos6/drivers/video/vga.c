/* vga.c */
#include <stdio.h>
#include <string.h>
#include <driver/video.h>

/* Some screen stuff. */
/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   24
/* The attribute of an character. */
#define ATTRIBUTE               7
/* The video memory address. */
#define VIDEO                   0xB8000

/* Variables. */
/* Save the X position. */
static int xpos;
/* Save the Y position. */
static int ypos;
/* Point to the video memory. */
unsigned char *video;
     
/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void cls(void)
{
	int i;
	
	video = (unsigned char *) VIDEO;
	for (i = 0; i < COLUMNS * LINES * 2; i++)
		*(video + i) = 0;
	xpos = 0;
	ypos = 0;
}

/* Put the character C on the screen. */
static void vga_putchar(char c)
{
	if (c == '\n') {
newline:
		xpos = 0;
		ypos++;
		if (ypos >= LINES)
		{
			ypos = LINES-1;
			memcpy((void*)VIDEO,(void*)(VIDEO+COLUMNS*2),ypos*COLUMNS*2);
			memset((void*)(VIDEO+ypos*COLUMNS*2),0,COLUMNS*2);
		}
		return;
	} else if (c == '\r') {
		xpos=0;
		memset((void*)(VIDEO+ypos*COLUMNS*2),0,COLUMNS*2);
		return;
	}
	
	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;
	xpos++;
 	if (xpos >= COLUMNS)
       		goto newline;
}

void vga_init(void)
{
	stdio_putchar=vga_putchar;
#include <apps/console/console.h>
	console_ll_putchar=vga_putchar;
}

