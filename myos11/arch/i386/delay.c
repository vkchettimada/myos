/* delay.c */
#include <stdio.h>
#include <asm/delay.h>

unsigned int timer_ticks;
static unsigned int loops_per_sec;

static void __delay(unsigned int loops)
{
	int d0;
	__asm__ __volatile__(
			"\tjmp 1f\n"
			".align 16\n"
			"1:\tjmp 2f\n"
			".align 16\n"
			"2:\tdecl %0\n"
			"\tjns 2b"
			: "=&a" (d0)
			: "0" (loops));
}

inline void udelay(unsigned int usecs)
{
	usecs*=0x000010c6;
	__asm__ __volatile__(
			"mull %0"
			: "=d" (usecs)
			: "a" (usecs), "0" (loops_per_sec));
	__delay(usecs);
}

void delay_init(void)
{
	int ticks;

	printk("delay_init... ");
	loops_per_sec = 1;
	while (loops_per_sec <<= 1) {
		ticks = timer_ticks;
		__delay(loops_per_sec);
		ticks = timer_ticks - ticks;
		if (ticks >= HZ/4) {
			__asm__("mull %1 ; divl %2"
				:"=a" (loops_per_sec)
				:"d" (HZ),
				 "r" (ticks),
				 "0" (loops_per_sec)
				);
			printk("ok - %d loops_per_sec\n",loops_per_sec);
			return;
		}
	}
	printk("failed\n");
}
	
