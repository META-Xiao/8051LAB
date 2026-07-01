#include <reg52.h>

sbit SDA = P0^2;
sbit SCL = P0^3;

#define fosc 24500000UL

typedef unsigned char uchar;

// 计算 5μs 对应的机器周期数（向上取整）
#define I2C_DELAY_CYCLES ((5UL*fosc+11999999UL)/12000000UL)

static void i2cDelay(void)
{
    uchar i;
    // 一个nop就是一个机器周期
    for(i=0; i<I2C_DELAY_CYCLES; i++) _nop_();
}

static void i2cStart(void)
{
    SDA = 1; SCL = 1; i2cDelay();
    SDA = 0;          i2cDelay();
    SCL = 0;
}

static void i2cStop(void)
{
    SDA = 0; SCL = 1; i2cDelay();
    SDA = 1;          i2cDelay();
}

static uchar i2cRecvAck(void)
{
    uchar ack;
    SDA = 1; i2cDelay();
    SCL = 1; i2cDelay();
    ack = SDA;
    SCL = 0; i2cDelay();
    return ack;
}

static void i2cSendAck(uchar ack)
{
    SDA = (ack) ? 1 : 0; i2cDelay();
    SCL = 1;               i2cDelay();
    SCL = 0;
}

static void i2cSendByte(uchar dat)
{
    uchar i;
    for (i = 0; i < 8; i++) {
        SDA = (dat & (0x80 >> i)) ? 1 : 0;
        i2cDelay();
        SCL = 1; i2cDelay();
        SCL = 0;
    }
}

static uchar i2cRecvByte(void)
{
    uchar i, dat = 0;
    SDA = 1;
    for (i = 0; i < 8; i++) {
        SCL = 1; i2cDelay();
        if (SDA) dat |= (0x80 >> i);
        SCL = 0; i2cDelay();
    }
    return dat;
}

static void writeReg(uchar reg, uchar dat)
{
    i2cStart();
    i2cSendByte(SLAVE_ADDR); i2cRecvAck();
    i2cSendByte(reg);         i2cRecvAck();
    i2cSendByte(dat);         i2cRecvAck();
    i2cStop();
}