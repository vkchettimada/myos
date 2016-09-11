/* entry.S */

#define SAVE_ALL	\
	cld;		\
	pushl %es;	\
	pushl %ds;	\
	pushl %eax;	\
	pushl %ebp;	\
	pushl %edi;	\
	pushl %esi;	\
	pushl %edx;	\
	pushl %ecx;	\
	pushl %ebx;

#define RESTORE_ALL	\
	popl %ebx;	\
	popl %ecx;	\
	popl %edx;	\
	popl %esi;	\
	popl %edi;	\
	popl %ebp;	\
	popl %eax;	\
	popl %ds;	\
	popl %es;

.data
	.global interrupt
interrupt:

.text

vector=0
.rept 256
	.align 4,0x90
1:	pushl $vector
	jmp common_irq_handler
.data
	.long 1b
.text
vector=vector+1
.endr

common_irq_handler:
	SAVE_ALL
	call irq_do
	RESTORE_ALL
	addl $4,%esp
	iret

