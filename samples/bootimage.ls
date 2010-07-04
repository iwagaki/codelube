OUTPUT_FORMAT("binary");
BASE_ADDR = 0;
SIGN_ADDR = 510;

SECTIONS
{
    . = BASE_ADDR;
    .boot_loader : {
    	ipl.o
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
    . = SIGN_ADDR;
    .boot_signature : {
        SHORT(0xaa55)
    }
}
