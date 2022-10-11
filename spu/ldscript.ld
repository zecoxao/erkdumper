ENTRY(_start)

SECTIONS {
	. = 0x25800;

	.text : {
		*(.text)
	}

	.data : {
		*(.data)
		*(.rodata)
	}

	.bss : {
		bss = .;
		*(.bss)
	}
}
