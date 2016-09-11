#define ASM 1
#include <boot/multiboot.h>

.text
.globl _start
_start:
	jmp multiboot_entry
	.align 4
multiboot_header:
	/* magic */
	.long MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_HEADER_FLAGS)

#ifndef __ELF__
	/* header_addr */
	.long multiboot_header
	/* load_addr */
	.long _start
	/* load_end_addr */
	.long _edata
	/* bss_end_addr */
	.long _end
	/* entry_addr */
	.long multiboot_entry
#endif

multiboot_entry:
	/* initialise the stack pointer */
	movl $(stack + STACK_SIZE),%esp
	/* reset flags */
	pushl $0
	popf
	/* push the pointer to multiboot information structure */
	pushl %ebx
	/* push the magic value */
	pushl %eax
	/* now enter the cmain function */
	call EXT_C(cmain)
	/* HALT */
	pushl $halt_message
	call EXT_C(printk)
loop:
	jmp loop
halt_message:
	.asciz "Halted."
	
	/* stack */
	.comm stack , STACK_SIZE

