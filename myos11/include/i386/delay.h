#ifndef _DELAY_H_
#define _DELAY_H_

#define HZ 1000

extern unsigned int timer_ticks;
extern unsigned int loops_per_sec;

inline void udelay(unsigned int usecs);
void delay_init(void);

#endif

