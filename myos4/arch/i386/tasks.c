/* tasks.c */
#include <asm/i386.h>
#include <asm/tasks.h>

extern desc_t *gdt;
extern tss_t *tss_task;

void task1(void);

void tasks_init(void)
{
	unsigned int sel[2];
	
	printk("tasks_init\n");
	
	__asm__ __volatile__ ("pushfl ; andl $0xffffbfff,(%esp) ; popfl");

	__asm__ __volatile__ ("\tltr %%ax\n"::"a" (5*8));
	printk("finished ltr\n");

	sel[1]=7*8;
	asm ("lcall %0"::"m" (*sel));
	
/*	__asm__ __volatile__ ("\tljmp %0,%1\n"
			: : "i" (6*8), "i" (0) );*/
/*	__asm__ ("\tjmp %%ax\n"
			: : "a" (&gdt[6]) );*/
	printk("tasks_init complete\n");
}

void task1(void)
{
	printk("task1 \nVinayak kariappa \nVinayak kariappa\n");
	while(1){
		printk("task1 \nVinayak kariappa \nVinayak kariappa\n");
	}
}

