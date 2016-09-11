
BOOTSEG=0x07C0
INITSEG=0x9000
PROGSEG=0x1000

.code16
.text
.global _start
_start:
	movw $BOOTSEG, %ax
	movw %ax, %ds		# %ds = BOOTSEG
	movw $INITSEG, %ax
	movw %ax, %es		# %ax = %es = INITSEG
	movw $512, %cx
	subw %si, %si
	subw %di, %di
	cld
	rep movsw
	ljmp $INITSEG, $go
		
go:
	movw %ax,%ds
	movw %ax,%ss
	movw $0xffff,%sp

	movw $PROGSEG,%ax
	movw %ax,%es		# dest seg
	movw $0x0000,%dx	# dh=side, dl=drive
	movw $0x0002,%cx	# ch=track, cl=sector(1-9)
	movw $0x0000,%bx	# dest offset
	movw $0x020f,%ax	# ah= 02 (read sector), al=sectors
	int $0x13
	call kill_motor

# mask all interrupts
	movb	$0xFF, %al	# mask all interrupts for now
	outb	%al, $0xA1
	call	delay
	
	movb	$0xFB, %al	# mask all irq's but irq2 which
	outb	%al, $0x21	# is cascaded

# enable A20 gate
	movw	$0x2401, %ax
	int	$0x15

# set up gdt
	xorl %eax,%eax		# Compute gdt_base
	movw %ds,%ax		# (Convert %ds:gdt to a linear ptr)
	shll $4,%eax
	addl $gdt,%eax
	movl %eax,(gdt_48+2)
	lgdt gdt_48		# load gdt with whatever is
				# appropriate

# switch to protected mode
	movw $1, %ax		# protected mode (PE) bit
	lmsw %ax		# This is it!
	jmp flush_instr

flush_instr:
	movw $0x18,%ax		# 00011000  0-1 RPL, 2-7 index into GDT (data)
	movw %ax,%ds
	movw %ax,%ss
	movl $0xffff,%esp
	.byte 0x66,0xea
	.long 0x10000
	.word 0x10		# 00010000 0-1 RPL, 2-7 index into GDT (code)

delay:
	outb	%al,$0x80
	ret

kill_motor:
	movw $0x3f2,%dx
	xorb %al,%al
	outb %al,%dx
	ret

/*****************************************************************************/
/* Data                                                                      */
/*****************************************************************************/
.align 4
gdt:
	.quad 0x0000000000000000	# NULL descriptor
	.quad 0x0000000000000000	# not used
	.quad 0x00CF9A000000FFFF	# base=0, limit=0xfffff, type=code
					# DPL=0, granularity=4k
	.quad 0x00CF92000000FFFF	# type=data

.align 4
gdt_48:
	.word	4*8-1		# gdt limit 
	.long	0		# gdt base (filled in later)

.org 510		
boot_flag:
	.word 0xAA55

