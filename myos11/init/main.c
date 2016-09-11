#include <stdio.h>
#include <stdlib.h>
#include <boot/multiboot.h>
#include <asm/mem.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <driver/i8253.h>
#include <driver/i8259a.h>
#include <driver/video.h>
#include <driver/pci.h>
#include <driver/lpc.h>
#include <driver/audio.h>
#include <driver/kbd.h>
#include <driver/media.h>
#include <driver/ide.h>
#include <myos/myos.h>
#include <myos/myos_irq.h>
#include <myos/myos_globals.h>

/* Macros. */
     
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
     
/* Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void cmain (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* initialise vga */
	vga_init();

	/* Clear the screen. */
	cls ();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printk ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}
	
	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;
	
	/* Print out the flags. */
	printk ("flags = 0x%x\n", (unsigned) mbi->flags);
	
	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printk ("mem_lower = %uKB, mem_upper = %uKB\n",(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
	
	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printk ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printk ("cmdline = %s\n", (char *) mbi->cmdline);
	
	/* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3)) {
		module_t *mod;
		int i;
		
		printk ("mods_count = %d, mods_addr = 0x%x\n",(int) mbi->mods_count, (int) mbi->mods_addr);
		for (i = 0, mod = (module_t *) mbi->mods_addr;
				i < mbi->mods_count;
				i++, mod += sizeof (module_t))
			printk (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
					(unsigned) mod->mod_start,
					(unsigned) mod->mod_end,
					(char *) mod->string);
	}
	
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)) {
		printk ("Both bits 4 and 5 are set.\n");
		return;
	}
	
	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG (mbi->flags, 4)) {
		aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);
		printk ("aout_symbol_table: tabsize = 0x%0x, strsize = 0x%x, addr = 0x%x\n",
				(unsigned) aout_sym->tabsize,
				(unsigned) aout_sym->strsize,
				(unsigned) aout_sym->addr);
	}
	
	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5)) {
		elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);
		printk ("elf_sec: num = %u, size = 0x%x, addr = 0x%x, shndx = 0x%x\n",
				(unsigned) elf_sec->num,
				(unsigned) elf_sec->size,
				(unsigned) elf_sec->addr,
				(unsigned) elf_sec->shndx);
	}
	
	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))	{
		memory_map_t *mmap;
		printk ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr,
				(unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size)))
			printk (" size = 0x%x, base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low,
					(unsigned) mmap->type);
	}

	/* initialise arch specific */
	mem_init();
	irq_init();

	/* initialise the OS */
	myOsInit();

	/* initialise the OS interrupts sub-system */
	myOsIrqInit(irq_default_handler);
	
	/* initialise tasks */
	{
		int i;

		for (i = 0; i < MYOS_N_TASKS; i++)
		{
			myOsAddTask(Tasks[i].entry, Tasks[i].stack_size, Tasks[i].priority);
		}
	}

	/* initialise PIC */
	i8259a_init();

	/* initialise timer */	
	i8253_init();
	
	/* enable interrupts */
//	irq_enable();

	/* calibrate delay loop */
//	delay_init();

	/* initialise drivers */
	kbd_init();
	pci_init();
	lpc_init();

	irq_enable();
	delay_init();
	
/*	audio_init();
	bt878a_init();
	ide_init();*/

	/* start the OS */
	myOsStart();

}

