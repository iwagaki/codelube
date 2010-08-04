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

    void putString(uint8_t x, uint8_t y, const char* str)
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
    char error_msg[] = "Error!";
    VRAMUtil vram;
    vram.putString(0, 0, error_msg);
}

uint8_t mapData[][18] = {
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
    {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
    {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
    {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
    {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2},
    {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7},
};


void drawScreen()
{
    VRAMUtil vram;

    // 枠
    for (uint8_t h = 0; h < 25; ++h)
    {
        for (int8_t w = 0; w < 80; ++w)
        {
            if ((h == 1 || h == 24) || ((h > 0) && (w == 0 || w == 79)))
                vram.putChar(w, h, 0x6020);
        }
    }

    // スコア
    vram.putString(0, 0, "SCORE: 0");

    // 面表示
    for (uint8_t h = 0; h < 10; ++h)
    {
        for (int8_t w = 0; w < 18; ++w)
        {
            if (mapData[h][w] != 0)
            {
                uint16_t col = 0x0020 | (mapData[h][w] << 12);
                vram.putChar(w * 4 + 4 + 0, h + 3, col);
                vram.putChar(w * 4 + 4 + 1, h + 3, col);
                vram.putChar(w * 4 + 4 + 2, h + 3, col);
                vram.putChar(w * 4 + 4 + 3, h + 3, col);
            }
        }
    }
}

bool isBlick(uint8_t x, uint8_t y)
{
    if ((x >= 4) && (x <= 17 * 4 + 4 + 3) && (y >= 3) && (y <= 9 + 3))
        return (mapData[y - 3][(x - 4) / 4] != 0);

    return false;
}

void clearBlick(uint8_t x, uint8_t y)
{
    VRAMUtil vram;

    if ((x >= 4) && (x <= 17 * 4 + 4 + 3) && (y >= 3) && (y <= 9 + 3))
    {
        uint8_t w = (x - 4) / 4;
        uint8_t h = y - 3;
        mapData[h][w] = 0;
        vram.putChar(w * 4 + 4 + 0, h + 3, 0x0020);
        vram.putChar(w * 4 + 4 + 1, h + 3, 0x0020);
        vram.putChar(w * 4 + 4 + 2, h + 3, 0x0020);
        vram.putChar(w * 4 + 4 + 3, h + 3, 0x0020);
    }
}


void showScore(int score)
{
    VRAMUtil vram;

    char string[] = "SCORE: 0000";

    int u1000 = score / 1000;
    int u100 =  (score % 1000) / 100;
    int u10 =  (score % 100) / 10;
    int u1 =  score % 10;

    string[7] = '0' + u1000;
    string[8] = '0' + u100;
    string[9] = '0' + u10;
    string[10] = '0' + u1;

    vram.putString(0, 0, string);
}


void main_func()
{

    VRAMUtil vram;
    drawScreen();

    int score = 0;

    int8_t x = 10, y = 20, vx = 1, vy = 1;
    
    for (;;)
    {
        showScore(score);

        vram.putChar(x, y, 0x0020);

        int8_t xx = x + vx;
        int8_t yy = y + vy;

        if (xx == 0 || xx == 79)
            vx = -vx;

        if (yy == 1 || yy == 24)
            vy = -vy;

        if (isBlick(xx, y))
        {
            clearBlick(xx, y);
            vx = -vx;
            score += 10;
        }

        if (isBlick(x, yy))
        {
            clearBlick(x, yy);
            vy = -vy;
            score += 10;
        }

        x = x + vx; y = y + vy;

        for (int i = 0; i < 100000; ++i)
            vram.putChar(x, y, 0x3020);
    }
}
