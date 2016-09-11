/* irq.c */
#include <asm/i386.h>
#include <asm/irq.h>
#include <asm/i8259a.h>

irq_gate_desc_t __attribute__((__section__(".data"))) idt[IDT_ENTRIES]={{0,},};
desc_load_t __attribute__((__section__(".data"))) idt_desc={
	.limit=IDT_ENTRIES*8-1,
	.desc=(desc_t*)idt	
};

static irq_t irqs[IDT_ENTRIES];

void irq_enable()
{
	__asm__ __volatile__ ("\tsti\n" : :);
}

void irq_disable()
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
//		printk("\rPANIC");
	}
}

void irq_do(regs_t regs, int irq)
{
	irqs[irq].count++;
	irqs[irq].handler(irq, &regs);
}

void irq_register(int irq, irq_handler_t handler)
{
	irqs[irq].handler=handler;
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

		irqs[i].handler=irq_default_handler;
	}

	__asm__ __volatile__ ("\tlidt %0\n" : : "m" (idt_desc));
}

