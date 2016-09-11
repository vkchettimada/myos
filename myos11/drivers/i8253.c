/* i8253.c */
#include <stdio.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <driver/i8259a.h>
#include <driver/i8253.h>
#include <myos/myos.h>
#include <myos/myos_irq.h>

#define TIMER_CLOCK_FREQ  14318180

#define TIMER_REG_IO_CNT0 0x40
#define TIMER_REG_IO_CNT1 0x41
#define TIMER_REG_IO_CNT2 0x42
#define TIMER_REG_TCW     0x43

static void i8253_irq(int irq, regs_t *regs)
{
	timer_ticks++;
	myOsTimer(irq,regs);
	i8259a_eoi();
}

void i8253_init()
{
	int count;
	
	printk("i8253_init\n");
	count = ((TIMER_CLOCK_FREQ/12) / HZ); /* per pulse count is decremented by 2 */
	outpb(0x36,TIMER_REG_TCW);
	outpb((count&0xff),TIMER_REG_IO_CNT0);
	outpb((count>>8),TIMER_REG_IO_CNT0);
	myOsIrqRegister(0x20,i8253_irq);
	timer_ticks = 0;
	i8259a_enable(0,0);
}

