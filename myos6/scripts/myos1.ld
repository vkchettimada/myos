ENTRY(_start)
SECTIONS
{
	. = 0x100000 + SIZEOF_HEADERS;
	.text : 
	{ 
		*(.text)
	}
	. = ALIGN(32 / 8);
	.rodata :
	{
		*(.rodata .rodata.*)
	}
	. = ALIGN(32 / 8);
	.data : 
	{ 
		*(.data)
	}
	. = ALIGN(32 / 8);
	.bss : 
	{ 
		*(.bss)
		. = ALIGN(32 / 8);
	}
	end = .;
}
