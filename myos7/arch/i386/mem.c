/* mem.c */
#include <stdio.h>
#include <asm/i386.h>

static desc_t __attribute__((__section__(".data"))) gdt[4] = {
	{{0},},
	{{0},},
	/* code segment */
	{(seg_desc_t){
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
	}},
	/* data segment */
	{(seg_desc_t){
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
	}},
};

static desc_load_t __attribute__((__section__(".data"))) gdt_desc={
	.limit=(4)*8-1,
	.desc=gdt
};

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

void mem_init(void)
{
	printk("mem_init\n");

	mem_lgdt();
}

