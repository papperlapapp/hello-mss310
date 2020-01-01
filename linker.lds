OUTPUT_ARCH(mips)

ENTRY(_start)

SECTIONS
{
    /* Our base address */
    /* . = STARTADDRESS; */

    /* Code */
    .text : {
        *(.text)
    }

    /* static data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    /* non-static data */
    .data : {
        *(.data*)
    }
    . = ALIGN(4);
    __stack = . + 0x1000;
}
