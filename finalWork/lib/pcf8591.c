/* PCF8591 AD/DA 模块驱动, I2C 位操作 */
#include "config.h"

#define PCF_ADDR_W 0x90
#define PCF_ADDR_R 0x91

static void i2cDelay(void)
{
    uchar i;
    for (i = 0; i < 30; i++);
}

static void i2cStart(void)
{
    PCF_SDA = 1; PCF_SCL = 1; i2cDelay();
    PCF_SDA = 0;              i2cDelay();
    PCF_SCL = 0;
}

static void i2cStop(void)
{
    PCF_SDA = 0; PCF_SCL = 1; i2cDelay();
    PCF_SDA = 1;              i2cDelay();
}

static uchar i2cRecvAck(void)
{
    uchar ack;
    PCF_SDA = 1; i2cDelay();
    PCF_SCL = 1; i2cDelay();
    ack = PCF_SDA;
    PCF_SCL = 0; i2cDelay();
    return ack;
}

static void i2cSendByte(uchar dat)
{
    uchar i;
    for (i = 0; i < 8; i++) {
        PCF_SDA = (dat & (0x80 >> i)) ? 1 : 0;
        i2cDelay();
        PCF_SCL = 1; i2cDelay();
        PCF_SCL = 0;
    }
}

static uchar i2cRecvByte(void)
{
    uchar i, dat = 0;
    PCF_SDA = 1;
    for (i = 0; i < 8; i++) {
        PCF_SCL = 1; i2cDelay();
        if (PCF_SDA) dat |= (0x80 >> i);
        PCF_SCL = 0; i2cDelay();
    }
    return dat;
}

static void i2cSendAck(uchar ack)
{
    PCF_SDA = ack ? 1 : 0; i2cDelay();
    PCF_SCL = 1;            i2cDelay();
    PCF_SCL = 0;
}

void pcf8591Init(void)
{
    P0MDOUT |=  0x08;
    P0MDOUT &= ~0x04;
    P0MDIN  |=  0x0C;
    PCF_SCL = 1;
    PCF_SDA = 1;
}

/**
 * 读取指定通道 ADC, 返回 8bit (0-255)
 * 流程: 写控制字(STOP) → 读dummy(NACK+STOP) → 读真实(NACK+STOP)
 */
uchar pcf8591ReadADC(uchar channel)
{
    uchar val;

    i2cStart();
    i2cSendByte(PCF_ADDR_W); i2cRecvAck();
    i2cSendByte(0x40 | (channel & 0x03)); i2cRecvAck();
    i2cStop();

    i2cStart();
    i2cSendByte(PCF_ADDR_R); i2cRecvAck();
    i2cRecvByte(); i2cSendAck(1);
    i2cStop();

    i2cStart();
    i2cSendByte(PCF_ADDR_R); i2cRecvAck();
    val = i2cRecvByte(); i2cSendAck(1);
    i2cStop();

    return val;
}

/**
 * 输出 DAC, value: 0-255
 */
void pcf8591WriteDAC(uchar value)
{
    i2cStart();
    i2cSendByte(PCF_ADDR_W); i2cRecvAck();
    i2cSendByte(0x40);        i2cRecvAck();
    i2cSendByte(value);       i2cRecvAck();
    i2cStop();
}

/**
 * 读取全部4通道, 存入 buf[4]
 */
void pcf8591ReadAll(uchar *buf)
{
    uchar i;

    i2cStart();
    i2cSendByte(PCF_ADDR_W); i2cRecvAck();
    i2cSendByte(0x44);        i2cRecvAck();

    i2cStart();
    i2cSendByte(PCF_ADDR_R); i2cRecvAck();
    i2cRecvByte(); i2cSendAck(0);

    for (i = 0; i < 4; i++) {
        buf[i] = i2cRecvByte();
        if (i < 3) i2cSendAck(0);
        else       i2cSendAck(1);
    }
    i2cStop();
}
