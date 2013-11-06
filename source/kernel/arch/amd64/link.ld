ENTRY(_start)
OUTPUT_ARCH(i386:x86-64)
OUTPUT_FORMAT("elf64-x86-64")

PHYS_BASE = 0x0000000000200000;
VIRT_BASE = 0xffffffff80000000;

SECTIONS
{
    . = VIRT_BASE + PHYS_BASE + SIZEOF_HEADERS;

    __text_start = .;
    .text :
    {
	*(.mb_header)
	*(.text)
	*(.text.*)
    }

    .rodata ALIGN(4096) :
    {
	*(.rodata)
	*(.rodata.*)
    }

    .data ALIGN(4096) :
    {
	*(.data)
	*(.data.*)
    }
    __data_end = .;

    .bss ALIGN(4096) :
    {
	*(.boot_bss)
	*(.bss)
	*(.bss.*)
    }

    . = ALIGN(4096);
    __kernel_end = .;
}
