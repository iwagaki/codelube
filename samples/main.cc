__asm__(".code16gcc"); // リアルモード用のコード

#include <stdint.h>

extern "C"
{
    extern void main_func(); // マングリングしないようにする
}

class VRAMUtil
{
public:
    VRAMUtil() : m_pos(0) {}

    void putString(uint8_t* str)
    {
        int ch = 0;
        while ((ch = *str++) != 0)
        {
            putChar(m_pos, ch);
            m_pos += 2;
        }
    }

    void pos(uint32_t pos)
    {
        m_pos = pos;
    }

private:
    void putChar(uint32_t addr, uint32_t ch)
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

    uint32_t m_pos;
};

void main_func()
{
    uint8_t asciiz[] = "hello,world!";

    VRAMUtil vram;

    vram.pos(0);
    vram.putString(asciiz);
}
