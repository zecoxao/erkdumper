OUTPUT_FORMAT("elf64-powerpc")
OUTPUT_ARCH(powerpc:common64)

__payload_base = 0x80000000007F0000;

ENTRY(_start)

PHDRS {
	text   PT_LOAD FLAGS(5);
	rodata PT_LOAD FLAGS(4);
	data   PT_LOAD FLAGS(6);
	bss    PT_LOAD FLAGS(6);
}

SECTIONS {
	. = __payload_base;

	__self_start = .;
		.start : {
			*(.start)
		} :text = 0

		. = ALIGN(32);
		.text : {
			*(.text)
			*(.text.*)
		}

		. = ALIGN(32);
		.opd : {
			*(.opd)
			*(.opd64)
		} :rodata = 0

		.toc : {
			__toc_start = .;
			*(.got)
			*(.toc)
		}

		.rodata : {
			*(.rodata)
			*(.rodata.*)
		}

		. = ALIGN(32);
		.data : {
			*(.data)
			*(.data.*)
			*(.sdata)
			*(.sdata.*)
		} :data = 0
	__self_end = .;

	__bss_start = .;
		.bss : {
			*(.sbss)
			*(.sbss.*)
			*(COMMON)
			*(.bss)
			*(.bss.*)
		} :bss = 0
	__bss_end = .;

	. = ALIGN(32);

	.stack : {
		_stack_top = .;
		. += 32K;
		_stack_bot = .;
	} :NONE

	__freemem = .;
}
