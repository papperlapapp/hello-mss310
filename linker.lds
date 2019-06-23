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

    /* Static data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    /* non-static data */
        .data : {
        *(.data*)
    }
}
