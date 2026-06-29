/**
 * oled7pin.c — SSD1306 OLED 128×64 7pin SPI 驱动
 */
#include "config.h"
#include "oledfont.h"

/* ===== 内部辅助 ===== */

/** 软件延时 ms (24.5MHz 下约 1ms) */
void oledDelayMs(uint ms)
{
    uint a;
    while (ms--) {
        a = 1800;
        while (a--);
    }
}

/**
 * 向 SSD1306 写一字节
 * dat: 数据, cmd: OLED_CMD(0)命令 / OLED_DATA(1)数据
 */
void oledWriteByte(uchar dat, uchar cmd)
{
    uchar i;

    if (cmd)
        OLED_DC_SET();
    else
        OLED_DC_CLR();

    OLED_CS_CLR();
    for (i = 0; i < 8; i++) {
        OLED_SCL_CLR();
        if (dat & 0x80)
            OLED_SDA_SET();
        else
            OLED_SDA_CLR();
        OLED_SCL_SET();
        dat <<= 1;
    }
    OLED_CS_SET();
    OLED_DC_SET();
}

/** 设置光标位置 (x:0~127, y:0~7页) */
static void oledSetPos(uchar x, uchar y)
{
    oledWriteByte(0xb0 + y, OLED_CMD);
    oledWriteByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    oledWriteByte((x & 0x0f) | 0x01, OLED_CMD);
}

/* ===== 公开 API ===== */

void oledDisplayOn(void)
{
    oledWriteByte(0x8D, OLED_CMD);
    oledWriteByte(0x14, OLED_CMD);
    oledWriteByte(0xAF, OLED_CMD);
}

void oledDisplayOff(void)
{
    oledWriteByte(0x8D, OLED_CMD);
    oledWriteByte(0x10, OLED_CMD);
    oledWriteByte(0xAE, OLED_CMD);
}

void oledClear(void)
{
    uchar i, n;
    for (i = 0; i < 8; i++) {
        oledWriteByte(0xb0 + i, OLED_CMD);
        oledWriteByte(0x00, OLED_CMD);
        oledWriteByte(0x10, OLED_CMD);
        for (n = 0; n < 128; n++)
            oledWriteByte(0x00, OLED_DATA);
    }
}

void oledInit(void)
{
    OLED_RST_SET();
    oledDelayMs(100);
    OLED_RST_CLR();
    oledDelayMs(100);
    OLED_RST_SET();

    oledWriteByte(0xAE, OLED_CMD); /* display off */
    oledWriteByte(0x00, OLED_CMD); /* set low column */
    oledWriteByte(0x10, OLED_CMD); /* set high column */
    oledWriteByte(0x40, OLED_CMD); /* start line */
    oledWriteByte(0x81, OLED_CMD); /* contrast */
    oledWriteByte(0xCF, OLED_CMD);
    oledWriteByte(0xA1, OLED_CMD); /* segment remap */
    oledWriteByte(0xC8, OLED_CMD); /* COM scan dir */
    oledWriteByte(0xA6, OLED_CMD); /* normal display */
    oledWriteByte(0xA8, OLED_CMD); /* MUX ratio */
    oledWriteByte(0x3F, OLED_CMD); /* 1/64 duty */
    oledWriteByte(0xD3, OLED_CMD); /* display offset */
    oledWriteByte(0x00, OLED_CMD);
    oledWriteByte(0xD5, OLED_CMD); /* clock div */
    oledWriteByte(0x80, OLED_CMD);
    oledWriteByte(0xD9, OLED_CMD); /* pre-charge */
    oledWriteByte(0xF1, OLED_CMD);
    oledWriteByte(0xDA, OLED_CMD); /* COM pins */
    oledWriteByte(0x12, OLED_CMD);
    oledWriteByte(0xDB, OLED_CMD); /* VCOMH */
    oledWriteByte(0x40, OLED_CMD);
    oledWriteByte(0x20, OLED_CMD); /* addressing mode */
    oledWriteByte(0x02, OLED_CMD); /* page mode */
    oledWriteByte(0x8D, OLED_CMD); /* charge pump */
    oledWriteByte(0x14, OLED_CMD);
    oledWriteByte(0xA4, OLED_CMD); /* entire disp off */
    oledWriteByte(0xA6, OLED_CMD); /* normal */
    oledWriteByte(0xAF, OLED_CMD); /* display on */

    oledClear();
    oledSetPos(0, 0);
}

void oledShowChar(uchar x, uchar y, uchar chr)
{
    uchar c, i;

    c = chr - ' ';
    if (x > OLED_COL_MAX - 1) { x = 0; y += 2; }

    oledSetPos(x, y);
    for (i = 0; i < 8; i++)
        oledWriteByte(f8x16[c * 16 + i], OLED_DATA);
    oledSetPos(x, y + 1);
    for (i = 0; i < 8; i++)
        oledWriteByte(f8x16[c * 16 + i + 8], OLED_DATA);
}

void oledShowStr(uchar x, uchar y, uchar *str)
{
    uchar j = 0;
    while (str[j] != '\0') {
        oledShowChar(x, y, str[j]);
        x += 8;
        if (x > 120) { x = 0; y += 2; }
        j++;
    }
}

void oledShowCn(uchar x, uchar y, uchar no)
{
    uchar t;

    oledSetPos(x, y);
    for (t = 0; t < 16; t++)
        oledWriteByte(Hzk[2 * no][t], OLED_DATA);

    oledSetPos(x, y + 1);
    for (t = 0; t < 16; t++)
        oledWriteByte(Hzk[2 * no + 1][t], OLED_DATA);
}

/** 窄字符版(8×16), 只写有效8字节, 间距8px */
void oledShowChar8(uchar x, uchar y, uchar no)
{
    uchar t;

    oledSetPos(x, y);
    for (t = 0; t < 8; t++)
        oledWriteByte(Hzk[2 * no][t], OLED_DATA);

    oledSetPos(x, y + 1);
    for (t = 0; t < 8; t++)
        oledWriteByte(Hzk[2 * no + 1][t], OLED_DATA);
}

static ulong oledPow10(uchar n)
{
    ulong r = 1;
    while (n--) r *= 10;
    return r;
}

void oledShowNum(uchar x, uchar y, ulong num, uchar len, uchar size)
{
    uchar t, temp, enshow = 0;

    for (t = 0; t < len; t++) {
        temp = (num / oledPow10(len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                oledShowChar(x + (size / 2) * t, y, ' ');
                continue;
            } else {
                enshow = 1;
            }
        }
        oledShowChar(x + (size / 2) * t, y, temp + '0');
    }
}

/**
 * 画 BMP, x0/y0 起始, x1 宽(像素), y1 高(页数, 8页=64像素)
 */
void oledDrawBMP(uchar x0, uchar y0, uchar x1, uchar y1, uchar code *bmp)
{
    uint j = 0;
    uchar x, y;

    for (y = y0; y < y0 + y1; y++) {
        oledSetPos(x0, y);
        for (x = x0; x < x1; x++)
            oledWriteByte(bmp[j++], OLED_DATA);
    }
}
