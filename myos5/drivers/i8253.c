/* i8253.c */
#include <stdio.h>
#include <driver/i8259a.h>
#include <driver/i8253.h>
#include <myos/myos.h>
#include <myos/myos_irq.h>

static void i8253_irq(int irq, regs_t *regs)
{
	myOsTimer(irq,regs);
	i8259a_eoi();
}

void i8253_init()
{
	printk("i8253_init\n");
	myOsIrqRegister(0x20,i8253_irq);
	i8259a_enable(0);
}

