/* irq.c */
#include <stdio.h>
#include <asm/i386.h>
#include <asm/irq.h>

static irq_gate_desc_t __attribute__((__section__(".data"))) idt[IDT_ENTRIES]={{0,},};
static desc_load_t __attribute__((__section__(".data"))) idt_desc={
	.limit=IDT_ENTRIES*8-1,
	.desc=(desc_t*)idt	
};

inline void irq_enable()
{
	__asm__ __volatile__ ("\tsti\n" : :);
}

inline void irq_disable()
{
	__asm__ __volatile__ ("\tcli\n" : :);
}

void irq_default_handler(int irq, regs_t *regs)
{
	static const char * const msg[] = {
		"divide error"
		,"debug exception"
		,"NMI"
		,"INT3"
		,"INTO"
		,"BOUND exception"
		,"invalid opcode"
		,"no coprocessor"
		,"double fault"
		,"coprocessor segment overrun"
		,"bad TSS"
		,"segment not present"
		,"stack fault"
		,"GPF"
		,"page fault"
		,"??"
		,"coprocessor error"
		,"alignment check"
		,"??","??","??", "??", "??", "??","??", "??", "??", "??","??", "??", "??", "??"
		,"IRQ0"
		,"IRQ1"
		,"IRQ2"
		,"IRQ3"
		,"IRQ4"
		,"IRQ5"
		,"IRQ6"
		,"IRQ7"
		,"IRQ8"
		,"IRQ9"
		,"IRQ10"
		,"IRQ11"
		,"IRQ12"
		,"IRQ13"
		,"IRQ14"
		,"IRQ15"
	};
	if (irq<0x20) {
		printk("irq %d (%s)\n",irq,msg[irq]);		
	} else {
		printk("irq %d\n",irq);
	}
	while(1) {
		printk("\rPANIC: unhandled interrupt");
	}
}

void irq_init()
{
	int i;

	printk("irq_init\n");

	for (i=0;i<IDT_ENTRIES;i++) {
		idt[i].p=1;
		idt[i].type=0xe;
		idt[i].selector=__KERNEL_CS__;
		idt[i].offset31_16=(unsigned int)interrupt[i]>>16;
		idt[i].offset15_0=(unsigned int)interrupt[i]&0xFFFF;
	}

	__asm__ __volatile__ ("\tlidt %0\n" : : "m" (idt_desc));
}

