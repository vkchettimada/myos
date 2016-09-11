/* myos_irq.c */
#include <myos/myos_irq.h>

static myOsIrq_t myos_irqs[MYOS_MAX_IRQ];

void myOsIrqHandler(regs_t regs, int irq)
{
	myos_irqs[irq].count++;
	myos_irqs[irq].handler(irq, &regs);
}

void myOsIrqRegister(int irq, myOsIrqHandler_t handler)
{
	myos_irqs[irq].handler=handler;
}

void myOsIrqInit(myOsIrqHandler_t arch_irq_default_handler)
{
	int i;

	for (i=0;i<MYOS_MAX_IRQ;i++) {
		myos_irqs[i].handler=arch_irq_default_handler;
	}
}

