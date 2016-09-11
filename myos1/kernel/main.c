#include "console.h"

void idle();
void worm();

void my_start()
{
	console_set_attr(0x07);
	console_set_pos(0,0);
	console_print("Hello world!!!");

	worm();
}

void idle()
{
idle_repeat:
	goto idle_repeat;
}

void worm()
{
	int x,y,xs,ys,i;
	
	x=y=1;
	xs=ys=1;
worm_repeat:
	console_set_pos(x,y);
	console_print("O");
	for (i=0;i<1000000;i++);
	console_set_pos(x,y);
	console_print(" ");
	x+=xs;
	y+=ys;
	if (x<=0 || x>=(CONSOLE_COLS-1))
		xs*=-1;
	if (y<=1 || y>=(CONSOLE_ROWS-1))
		ys*=-1;
	goto worm_repeat;
}
