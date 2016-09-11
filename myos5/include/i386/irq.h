/* irq.h */
#ifndef _IRQ_H_
#define _IRQ_H_

#include <asm/i386.h>

#define IDT_ENTRIES 256

void irq_enable();
void irq_disable();
void irq_default_handler(int irq, regs_t *regs);
void irq_init();
#endif

