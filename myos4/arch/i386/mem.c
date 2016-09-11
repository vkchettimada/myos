/* mem.c */
#include <asm/i386.h>
#include <asm/tasks.h>


#define offsetof(type,member) ((unsigned short) &((type*)0)->member)

desc_t __attribute__((__section__(".data"))) gdt[4+TASKS_MAX*2] = {
	{0,},
	/* common ldt */
	(seg_desc_t)/*(sys_desc_t)*/{
		.limit15_0=4*8-1,
		//.type=0x2,
		.rw=0x1,
		.p=0x1,
		.g=0x1,
	},
	/* code segment */
	(seg_desc_t){
		.limit15_0=0xFFFF,
		.b15_0=0x0000,
		.b23_16=0x0000,
		.a=0x00,
		.rw=0x01,
		.x=0x00,
		.e=0x01,
		.s=0x01,
		.dpl=0x00,
		.p=0x01,
		.limit19_16=0x0F,
		.avl=0x00,
		.o=0x00,
		.d=0x01,
		.g=0x01,
		.b31_24=0x0000,
	},
	/* data segment */
	(seg_desc_t){
		.limit15_0=0xFFFF,
		.b15_0=0x0000,
		.b23_16=0x0000,
		.a=0x00,
		.rw=0x01,
		.x=0x00,
		.e=0x00,
		.s=0x01,
		.dpl=0x00,
		.p=0x01,
		.limit19_16=0x0F,
		.avl=0x00,
		.o=0x00,
		.d=0x01,
		.g=0x01,
		.b31_24=0x0000,
	},
};

desc_load_t __attribute__((__section__(".data"))) gdt_desc={
	.limit=(4+TASKS_MAX*2)*8-1,
	.desc=gdt
};

extern void task1();
	
desc_t __attribute__((__section__(".data"))) common_ldt[4];
	
tss_t __attribute__((__section__(".data"))) tss_task[TASKS_MAX] = {
	{
		.eflags=0x00000002,
		.es=__KERNEL_DS__,
		.cs=__KERNEL_CS__,
		.ss=__KERNEL_DS__,
		.ds=__KERNEL_DS__,
		.fs=__KERNEL_DS__,
		.gs=__KERNEL_DS__,
		.ss0=__KERNEL_DS__,
		.t=1,
		.ldt=0x08,
		.bitmap_offset=offsetof(tss_t,io_bitmap),
	},
	{
		.eflags=0x00000002,
		.es=__KERNEL_DS__,
		.cs=__KERNEL_CS__,
		.ss=__KERNEL_DS__,
		.ds=__KERNEL_DS__,
		.fs=__KERNEL_DS__,
		.gs=__KERNEL_DS__,
		.ss0=__KERNEL_DS__,
		.esp0=0xfffff000,
		.esp=0xfffff000,
		.eip=(unsigned int)task1,
		.t=1,
		.ldt=0x08,
		.bitmap_offset=offsetof(tss_t,io_bitmap),
	},
};

unsigned char io_bitmap[0x2000]={0,};

void dump(void *m,int c)
{
	int i;

	for (i=0;i<c;i++) {
		if (i%8==0)
			printk("\n");
		printk("%x ",((unsigned char*)m)[i]);
	}
}

static void mem_lgdt(void)
{
	__asm__ __volatile__ ("\tlgdt %0;\n \
			\tmov %1,%%ax\n \
			\tmov %%ax,%%ds\n \
			\tmov %%ax,%%es\n \
			\tmov %%ax,%%ss\n \
			\tmov %%ax,%%fs\n \
			\tmov %%ax,%%gs\n \
			\tljmp %2,$longjmp\n \
			longjmp:\tnop\n"
		       	: 
			: "m" (gdt_desc), "i" (__KERNEL_DS__), "i" (__KERNEL_CS__));
}

void tasks_mem_init(void)
{
	unsigned int tss,ildt;
	int i;

	printk("tasks_mem_init\n");
	printk("&gdt= 0x%x\n",(unsigned int)gdt);

	for (i=0;i<TASKS_MAX;i++) {

		gdt[4+i*2].task_gate.selector=(4+i*2+1)*sizeof(tss_desc_t);
		gdt[4+i*2].task_gate.type=0x5;
		gdt[4+i*2].task_gate.dpl=0x0;
		gdt[4+i*2].task_gate.p=0x1;

		tss=(unsigned int)&tss_task[i];
		gdt[4+i*2+1].tss.limit15_0=0xffff;
		gdt[4+i*2+1].tss.b15_0=tss&0xffff;
		gdt[4+i*2+1].tss.b23_16=(tss>>16)&0xff;
		gdt[4+i*2+1].tss.type=0x9;
		gdt[4+i*2+1].tss.dpl=0x0;
		gdt[4+i*2+1].tss.p=0x1;
		gdt[4+i*2+1].tss.limit19_16=0xff;
		gdt[4+i*2+1].tss.avl=0x1;
		gdt[4+i*2+1].tss.g=0x1;
		gdt[4+i*2+1].tss.b31_24=(tss>>24)&0xff;
		__asm__ __volatile__ ("movl %%cr3,%%eax; movl %%eax, %0" : "=m" (tss_task[i].cr3):);
		printk("%d.cr3 = 0x%x\n",i,tss_task[i].cr3);
	}
	memcpy(common_ldt,gdt,sizeof(desc_t)*4);
	memset(&common_ldt[1],0,sizeof(desc_t));
	ildt=(unsigned int)&common_ldt[0];
	gdt[1].sys.b15_0=ildt&0xffff;
	gdt[1].sys.b23_16=(ildt>>16)&0xff;
	gdt[1].sys.b31_24=(ildt>>24)&0xff;
}

void mem_init()
{
	printk("mem_init\n");

	printk("sizeof gdt=%d\n",sizeof(desc_t));
	printk("gdt[4] addr= 0x%x\n",&gdt[4]);
	
	tasks_mem_init();
	
	dump(gdt,(4+TASKS_MAX*2)*8);

	mem_lgdt();
}

