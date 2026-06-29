/**
 * DS18B20 1-Wire 时序 (24.5MHz/12 ≈ 2.04MHz 机器周期):
 *   delay(n)       ≈ n * 0.5us
 *   delay10us(n)   ≈ n * 10us (调用 delay(18))
 */

#include "config.h"

/* ===== 内部延时 ===== */
static void delay(uint n)
{
    while (n--);
}

static void delay10us(uchar n)
{
    uchar i;
    for (i = 0; i < n; i++) delay(18);
}

/* ===== 1-Wire 复位 ===== */
static uchar owReset(void)
{
    uchar presence;
    DQ = 0;
    delay10us(50);
    DQ = 1;
    delay10us(5);
    presence = DQ;
    delay10us(50);
    return presence;
}

/* ===== 1-Wire 写字节 ===== */
static void owWrite(uchar dat)
{
    uchar i;
    for (i = 0; i < 8; i++) {
        DQ = 0;
        delay(2);
        DQ = dat & 0x01;
        delay10us(6);
        DQ = 1;
        dat >>= 1;
        delay(2);
    }
}

/* ===== 1-Wire 读字节 ===== */
static uchar owRead(void)
{
    uchar i, dat = 0;
    for (i = 0; i < 8; i++) {
        DQ = 0;
        delay(2);
        dat >>= 1;
        DQ = 1;
        delay(2);
        if (DQ) dat |= 0x80;
        delay10us(5);
    }
    return dat;
}

/* ===== API ===== */

void ds18b20Init(void)
{
    DQ = 1;
}

void ds18b20Start(void)
{
    owReset();
    owWrite(0xCC);
    owWrite(0x44);
}

uint ds18b20Read(void)
{
    uchar lo, hi;
    uint raw16;

    owReset();
    owWrite(0xCC);
    owWrite(0xBE);

    lo = owRead();
    hi = owRead();

    raw16 = ((uint)hi << 8) | lo;

    return (uint)((raw16 * 100UL) / 16);
}

static void waitMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

uint ds18b20ReadTemp(void)
{
    ds18b20Start();
    waitMs(750);
    return ds18b20Read();
}
