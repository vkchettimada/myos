/* irq.h */
#ifndef _IRQ_H_
#define _IRQ_H_
#include <asm/i386.h>

#define IDT_ENTRIES 256

typedef void (*irq_handler_t)(int irq, regs_t *regs);

typedef struct {
	unsigned int count;
	irq_handler_t handler;
} irq_t;

extern void (*interrupt[IDT_ENTRIES])(void);

#endif
