/* vga.h */
#ifndef _VGA_H_
#define _VGA_H_

/* Some screen stuff. */
/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   24
/* The attribute of an character. */
#define ATTRIBUTE               7
/* The video memory address. */
#define VIDEO                   0xB8000

extern int xpos,ypos;
extern unsigned char *video;

void cls(void);
void vga_init(void);

#endif
