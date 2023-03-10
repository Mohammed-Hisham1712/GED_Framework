ENTRY(reset_handler)

MEMORY
{
	FLASH(rx):ORIGIN =0x08000000,LENGTH =512K
	SRAM(wx):ORIGIN =0x20000000,LENGTH =128K
}

SECTIONS
{
	.text :
	{
		*(.reset_vector)
		KEEP(*(.reset_vector))	/* Keep this sections even if its symbols are not referenced */
		*(.text)
		*(.text.*)
		*(.init)
		*(.fini)
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(4);
	}> FLASH

	_la_data = LOADADDR(.data);

	.data :
	{
		_sdata = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		_edata = .;
	}> SRAM AT> FLASH

	/DISCARD/ :
	{
		*(.init_array)
		*(.fini_array)
		*(.init_array.00000)
	}
		
	.bss :
	{
		_sbss = .;
		__bss_start__ = _sbss;	/* for newlib-nono support */
		*(.bss)		/* Data initialized with 0 or static variables */
		*(.bss.*)
		*(COMMON)	/* COMMON segment is where uninitialized global variable are store after linking */
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = _ebss;	/* For newlib-nano support */
		__end__ = _ebss;	/* Support for _sbrk */
		end = _ebss;		/* For rdimon.specs */
	}> SRAM AT> SRAM
}
