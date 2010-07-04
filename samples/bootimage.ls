OUTPUT_FORMAT("binary");
BASE = 0x0000;

SECTIONS
{
    . = BASE;
    .ipl : {
    	ipl.o
    }
}
