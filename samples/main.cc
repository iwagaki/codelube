__asm__(".code16gcc"); // リアルモード用のコード

#include <stdint.h>

extern "C"
{
    extern void main_func(); // マングリングしないようにする
    extern void error_func(); // マングリングしないようにする
}

class VRAMUtil
{
public:
    void putChar(uint8_t x, uint8_t y, uint16_t ch)
    {
        writeChar(xyToAddr(x, y), ch);
    }

    void putString(uint8_t x, uint8_t y, uint8_t* str)
    {
        int ch = 0;
        while ((ch = *str++) != 0)
            writeChar(xyToAddr(x++, y), ch | 0x8300);
    }

private:
    uint32_t xyToAddr(uint8_t x, uint8_t y)
    {
        return (y * 80 + x) * 2;
    }

    void writeChar(uint32_t addr, uint32_t ch)
    {
        __asm__(
            "push %%es;"
            "push %%ax;"
            "push %%bx;"
            "push %%dx;"
            "movw $0xb800, %%dx;"
            "movw %%dx, %%es;"
            // "movb $0xec, %%ah;"
            "movw %%ax, %%es:(%%bx);"
            "pop %%dx;"
            "pop %%bx;"
            "pop %%ax;"
            "pop %%es;"
            :
            : "b"(addr), "a"(ch)
            );
    }
};

void error_func()
{
    uint8_t error_msg[] = "Error!";
    VRAMUtil vram;
    vram.putString(0, 0, error_msg);
}

void main_func()
{
    VRAMUtil vram;

    for (uint8_t h = 0; h < 25; ++h)
    {
        for (int8_t w = 0; w < 80; ++w)
        {
            if ((h == 0 || h == 24) || (w == 0 || w == 79))
                vram.putChar(w, h, 0x6020);
            if (((h % 3 == 2) && h < 12) && ((w % 10) > 1 && (w % 10) < 8))
                vram.putChar(w, h, 0x2020);
        }
    }

    int8_t x = 10, y = 10, vx = 1, vy = 1;
    
    for (;;)
    {
        vram.putChar(x, y, 0x0020);
        x += vx; y += vy;
        for (int i = 0; i < 1000000; ++i)
            vram.putChar(x, y, 0x3020);

        if (x == 1 || x == 78)
            vx = -vx;

        if (y == 1 || y == 23)
            vy = -vy;
    }
}
