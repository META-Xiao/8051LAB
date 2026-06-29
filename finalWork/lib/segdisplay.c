#include "config.h"
#include "intrins.h"

static uchar code segTable[10] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

static uchar code segHexTable[16] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90,
    0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};

#define SEG_DASH   0xBF
#define SEG_BLANK  0xFF

static uchar dispBuf[SEG_DIGIT_COUNT] = {0xFF, 0xFF, 0xFF, 0xFF};
static uchar scanIdx = 0;

void segInit(void)
{
    _SEG_CFG();
    SEG_DIGIT0 = 1;
    SEG_DIGIT1 = 1;
    SEG_DIGIT2 = 1;
    SEG_DIGIT3 = 1;
    SEG_PORT = 0xFF;
    dispBuf[0] = SEG_BLANK;
    dispBuf[1] = SEG_BLANK;
    dispBuf[2] = SEG_BLANK;
    dispBuf[3] = SEG_BLANK;
    scanIdx = 0;
}

uchar segCode(uchar digit)
{
    return (digit > 9) ? SEG_BLANK : segTable[digit];
}

uchar segHexCode(uchar val)
{
    return segHexTable[val & 0x0F];
}

void segShowBuf(uchar *bufp, uchar len)
{
    uchar i;
    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        dispBuf[i] = (i < len) ? bufp[i] : SEG_BLANK;
    }
}

void segShow(uint num)
{
    uchar d[SEG_DIGIT_COUNT];
    uchar i, leading = 1;

    if (num > 9999) {
        for (i = 0; i < SEG_DIGIT_COUNT; i++) dispBuf[i] = SEG_DASH;
        return;
    }

    d[0] = (uchar)(num / 1000);
    d[1] = (uchar)((num / 100) % 10);
    d[2] = (uchar)((num / 10) % 10);
    d[3] = (uchar)(num % 10);

    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        if (i == SEG_DIGIT_COUNT - 1) {
            dispBuf[i] = segTable[d[i]];
        } else if (leading && d[i] == 0) {
            dispBuf[i] = SEG_BLANK;
        } else {
            leading = 0;
            dispBuf[i] = segTable[d[i]];
        }
    }
}

void segShowHex(uint num)
{
    uchar i;
    for (i = 0; i < SEG_DIGIT_COUNT; i++) {
        dispBuf[i] = segHexTable[(num >> (12 - 4 * i)) & 0x0F];
    }
}

void segShowRaw(uchar pos, uchar seg)
{
    SEG_PORT = seg;
    switch (pos) {
        case 0: SEG_DIGIT0 = 0; break;
        case 1: SEG_DIGIT1 = 0; break;
        case 2: SEG_DIGIT2 = 0; break;
        case 3: SEG_DIGIT3 = 0; break;
        default: return;
    }
    segDelayMs(2);
    SEG_DIGIT0 = 1;
    SEG_DIGIT1 = 1;
    SEG_DIGIT2 = 1;
    SEG_DIGIT3 = 1;
    SEG_PORT = 0xFF;
}

void segScan(void)
{
    SEG_PORT = 0xFF;
    SEG_DIGIT0 = 1;
    SEG_DIGIT1 = 1;
    SEG_DIGIT2 = 1;
    SEG_DIGIT3 = 1;

    if (dispBuf[scanIdx] != SEG_BLANK) {
        SEG_PORT = dispBuf[scanIdx];
        switch (scanIdx) {
            case 0: SEG_DIGIT0 = 0; break;
            case 1: SEG_DIGIT1 = 0; break;
            case 2: SEG_DIGIT2 = 0; break;
            case 3: SEG_DIGIT3 = 0; break;
        }
    }

    scanIdx++;
    if (scanIdx >= SEG_DIGIT_COUNT) scanIdx = 0;
}

void segClear(void)
{
    SEG_DIGIT0 = 1;
    SEG_DIGIT1 = 1;
    SEG_DIGIT2 = 1;
    SEG_DIGIT3 = 1;
    SEG_PORT = 0xFF;
    dispBuf[0] = SEG_BLANK;
    dispBuf[1] = SEG_BLANK;
    dispBuf[2] = SEG_BLANK;
    dispBuf[3] = SEG_BLANK;
    scanIdx = 0;
}

void segDelayMs(uint ms)
{
    uchar j;
    while (ms--) {
					for (j = 0; j < 250; j++) {
            _nop_();
        }
    }
}
