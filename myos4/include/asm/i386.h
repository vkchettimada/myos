/* i386.h */

#ifndef _I386_H_
#define _I386_H_

#pragma pack(1)

typedef struct {
	unsigned short limit15_0;
	unsigned short b15_0;
	unsigned char b23_16;
	
	unsigned char a:1;
	unsigned char rw:1;
	unsigned char x:1;
	unsigned char e:1;
	unsigned char s:1;
	unsigned char dpl:2;
	unsigned char p:1;
	
	unsigned char limit19_16:4;
	unsigned char avl:1;
	unsigned char o:1;
	unsigned char d:1;
	unsigned char g:1;
	
	unsigned char b31_24;
} seg_desc_t;

typedef struct {
	unsigned short limit15_0;
	unsigned short b15_0;
	unsigned char b23_16;
	
	unsigned char type:4;
	unsigned char o1:1;
	unsigned char dpl:2;
	unsigned char p:1;
	
	unsigned char limit19_16:4;
	unsigned char o2:3;
	unsigned char g:1;
	
	unsigned char b31_24;
} sys_desc_t;

typedef struct {
	unsigned short limit15_0;
	unsigned short b15_0;
	unsigned char b23_16;
	
	unsigned char type:4;
	unsigned char o1:1;
	unsigned char dpl:2;
	unsigned char p:1;
	
	unsigned char limit19_16:4;
	unsigned char avl:1;
	unsigned char o2:2;
	unsigned char g:1;
	
	unsigned char b31_24;
} tss_desc_t;

typedef struct {
	unsigned short offset15_0;
	unsigned short selector;

	unsigned char wc:4;
	unsigned char o1:4;
	
	unsigned char type:4;
	unsigned char o2:1;
	unsigned char dpl:2;
	unsigned char p:1;
	
	unsigned short offset31_16;
} irq_gate_desc_t;

typedef struct {
	unsigned short resv0;
	unsigned short selector;

	unsigned char resv1;
	
	unsigned char type:4;
	unsigned char o1:1;
	unsigned char dpl:2;
	unsigned char p:1;
	
	unsigned short resv2;
} task_gate_desc_t;

typedef union {
	seg_desc_t seg;
	sys_desc_t sys;
	tss_desc_t tss;
	irq_gate_desc_t irq_gate;
	task_gate_desc_t task_gate;
} desc_t;

typedef struct {
	unsigned short limit;
	desc_t *desc;
} desc_load_t;

typedef struct {
	unsigned short back_link;
	unsigned short resv0;
	unsigned int esp0;
	unsigned short ss0;
	unsigned short resv1;
	unsigned int esp1;
	unsigned short ss1;
	unsigned short resv2;
	unsigned int esp2;
	unsigned short ss2;
	unsigned short resv3;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned short es;
	unsigned short resv4;
	unsigned short cs;
	unsigned short resv5;
	unsigned short ss;
	unsigned short resv6;
	unsigned short ds;
	unsigned short resv7;
	unsigned short fs;
	unsigned short resv8;
	unsigned short gs;
	unsigned short resv9;
	unsigned short ldt;
	unsigned short resv10;
	unsigned short t:1;
	unsigned short resv11:15;
	unsigned short bitmap_offset;
	unsigned long io_bitmap[32+1];
} tss_t;

typedef struct {
	unsigned int _ebx;
	unsigned int _ecx;
	unsigned int _edx;
	unsigned int _esi;
	unsigned int _edi;
	unsigned int _ebp;
	unsigned int _eax;
	unsigned int _ds;
	unsigned int _es;
} regs_t;

#define __KERNEL_DS__ 0x18
#define __KERNEL_CS__ 0x10

#endif
