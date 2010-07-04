#include <stdint.h>
__asm__(".code16gcc");

void put_char(uint32_t addr, uint32_t ch)
{
    __asm__(
        "push %%es;"
        "push %%ax;"
        "push %%bx;"
        "push %%dx;"
        "movw $0xb800, %%dx;"
        "movw %%dx, %%es;"
        "movb $0xec, %%ah;"
        "movw %%ax, %%es:2(%%bx);"
        "pop %%dx;"
        "pop %%bx;"
        "pop %%ax;"
        "pop %%es;"
        :
        : "b"(addr), "a"(ch)
        );
}

void main()
{
    uint8_t asciiz[] = "hello,world!";
    uint8_t* src = asciiz;
    uint32_t dst = 0;
    uint32_t ch = 0;

    while ((ch = *src++) != 0)
    {
        put_char(dst, ch);
        dst += 2;
    }
}
