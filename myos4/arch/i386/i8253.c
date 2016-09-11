/* i8253.c */
#include <asm/irq.h>
#include <asm/i8259a.h>
#include <myos/vga.h>

void (*i8253_hook)(void)=0;

void i8253_irq(regs_t regs, int irq)
{
	(*(unsigned char*)video)++;
	if (i8253_hook)
		(*i8253_hook)();
	i8259a_eoi();
}

void i8253_init()
{
	printk("i8253_init\n");
	irq_register(0x20,i8253_irq);
	i8259a_enable(0);
}

