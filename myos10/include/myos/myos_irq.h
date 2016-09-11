/* myos_irq.h */
#ifndef _MYOS_IRQ_H_
#define _MYOS_IRQ_H_

#ifdef ARCH_I386
	#include <asm/i386.h>
	#define MYOS_MAX_IRQ IDT_ENTRIES
#endif

typedef void (*myOsIrqHandler_t)(int irq, regs_t *regs);

typedef struct {
	unsigned int count;
	myOsIrqHandler_t handler;
} myOsIrq_t;

void myOsIrqRegister(int irq, myOsIrqHandler_t handler);
void myOsIrqInit();

#endif

