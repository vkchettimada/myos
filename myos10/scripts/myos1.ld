ENTRY(_start)
PHDRS
{
	/*headers PT_PHDR FILEHDR PHDRS;*/
	text PT_LOAD FLAGS(0x5);
	data PT_LOAD FLAGS(0x6);
}
SECTIONS
{
	/*. = SIZEOF_HEADERS;*/
	.text 0x100000 : AT(0x100000)
	{
		*(.text*)
		*(.rodata*)
	} : text
	.data ALIGN(4) : 
	{
		*(.data*)
	} : data
	.bss ALIGN(4) : 
	{
		*(.bss*)
		*(COMMON)
	}
	end = .;
	/DISCARD/ :
	{
		*(.*)
	}
}

