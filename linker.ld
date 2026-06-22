ENTRY(loader)
SECTIONS
{
    . = 0x00100000;
    .text :
    {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }
    .rodata : { *(.rodata) }
    .data :   { *(.data)   }
    .bss :
    {
        *(.bss)
        *(COMMON)
    }
}
