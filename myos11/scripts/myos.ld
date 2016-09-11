ENTRY(_start)
PHDRS
{
	TEXT PT_LOAD FLAGS(5);
	DATA PT_LOAD FLAGS(6);
}
SECTIONS
{
	. = 0x100000;/* + SIZEOF_HEADERS;*/
	.text :	{ *(.text) *(.rodata*) } : TEXT
	.data ALIGN(4) : { *(.data) } : DATA
	.bss ALIGN(4) : { *(.bss) }
	_end = .;
	/DISCARD/ : { *(.*) }
}

