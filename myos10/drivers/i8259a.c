#include <stdio.h>
#include <driver/i8259a.h>
#include <myos/myos_io.h>

void i8259a_eoi()
{
	outpb(0x20,PIC_MASTER_OCW2);
}

void i8259a_enable(int irq, int level)
{
	int mask, elcr;
	
	if (irq<8) {
		mask=inpb(PIC_MASTER_OCW1);
		mask&=~(1<<irq);
		outpb(mask,PIC_MASTER_OCW1);
		elcr=inpb(PIC_MASTER_ELCR);
		elcr |= (((level)?1:0)<<irq);
		outpb(elcr,PIC_MASTER_ELCR);
	} else {
		mask=inpb(PIC_SLAVE_OCW1);
		mask&=~(1<<(irq-8));
		outpb(mask,PIC_SLAVE_OCW1);
		elcr=inpb(PIC_SLAVE_ELCR);
		elcr |= (((level)?1:0)<<(irq-8));
		outpb(elcr,PIC_SLAVE_ELCR);
	}
}

void i8259a_disable(int irq)
{
	int mask;
	if (irq<8) {
		mask=inpb(PIC_MASTER_OCW1);
		mask|=(1<<irq);
		outpb(mask,PIC_MASTER_OCW1);
	} else {
		mask=inpb(PIC_SLAVE_OCW1);
		mask|=(1<<(irq-8));
		outpb(mask,PIC_SLAVE_OCW1);
	}
}

void i8259a_init()
{
	printk("i8259a_init\n");

	/* ICW1 */
	outpb(0x11,PIC_MASTER_ICW1);
	outpb(0x11,PIC_SLAVE_ICW1);

	/* ICW2 */
	outpb(0x20,PIC_MASTER_ICW2);
	outpb(0x28,PIC_SLAVE_ICW2);

	/* ICW3 */
	outpb(0x04,PIC_MASTER_ICW3);
	outpb(0x02,PIC_SLAVE_ICW3);

	/* ICW4 */
	outpb(0x01,PIC_MASTER_ICW4);
	outpb(0x01,PIC_SLAVE_ICW4);

	/* disable irqs */
	outpb(~0x00,PIC_MASTER_OCW1);
	outpb(~0x00,PIC_SLAVE_OCW1);
}

