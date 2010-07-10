OUTPUT_FORMAT("binary");

SECTIONS
{
    . = 0;
    .boot_loader : {
    	ipl.o
    }
    . = 446;
    .partition_table : {
        BYTE(0x80) /* boot flag */
        BYTE(0x00) /* start CHS  H */
        BYTE(0x01) /* start CHS  [5:0] -> S, [6:7] -> C[8:9] */
        BYTE(0x00) /* start CHS  C[0:7] */
        BYTE(0x0c) /* partition type */
        BYTE(0x00) /* end CHS (TBD) */
        BYTE(0x3f)
        BYTE(0x00)
	SHORT(0x0000) /* start LBA */
	SHORT(0x0000)
	SHORT(0x003f) /* end LBA (TBD) */
	SHORT(0x0000)
    }
    . = 510;
    .boot_signature : {
        SHORT(0xaa55)
    }
    . = 512;
    .kernel : { /* should be placed here */
        kernel.o
    }
    .text : {
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}
