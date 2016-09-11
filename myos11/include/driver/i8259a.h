/* i8259a.h */
#ifndef _I8259A_H_
#define _I8259A_H_

#define PIC_MASTER_ICW1	0x20
#define PIC_MASTER_ICW2	0x21
#define PIC_MASTER_ICW3	0x21
#define PIC_MASTER_ICW4	0x21
#define PIC_MASTER_OCW1 0x21
#define PIC_MASTER_OCW2 0x20
#define PIC_MASTER_OCW3 0x20
#define PIC_MASTER_ELCR 0x4D0

#define PIC_SLAVE_ICW1	0xa0
#define PIC_SLAVE_ICW2	0xa1
#define PIC_SLAVE_ICW3	0xa1
#define PIC_SLAVE_ICW4	0xa1
#define PIC_SLAVE_OCW1	0xa1
#define PIC_SLAVE_OCW2	0xa0
#define PIC_SLAVE_OCW3	0xa0
#define PIC_SLAVE_ELCR 0x4D1

void i8259a_eoi();
void i8259a_enable(int irq, int level);
void i8259a_disable(int irq);
void i8259a_init();

#endif
