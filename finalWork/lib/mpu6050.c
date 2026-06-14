#include "mpu6050.h"
#include <math.h>

#define REG_SMPLRT_DIV   0x19
#define REG_CONFIG       0x1A
#define REG_GYRO_CONFIG  0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B
#define REG_PWR_MGMT_1   0x6B
#define SLAVE_ADDR       0xD0

static void i2cDelay(void)
{
    uchar i;
    for (i = 0; i < 30; i++);
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

void mpu6050Init(void)
{
    P0MDOUT |= 0x08;
    P0MDOUT &= ~0x04;
    P0MDIN  |= 0x0C;
    SCL = 1; SDA = 1;

    writeReg(REG_PWR_MGMT_1,  0x01);
    writeReg(REG_SMPLRT_DIV,  0x09);
    writeReg(REG_CONFIG,      0x06);
    writeReg(REG_GYRO_CONFIG, 0x18);
    writeReg(REG_ACCEL_CONFIG, 0x18);
}

void mpu6050ReadRegs(uchar reg, uchar *buf, uchar len)
{
    uchar i;

    i2cStart();
    i2cSendByte(SLAVE_ADDR); i2cRecvAck();
    i2cSendByte(reg);         i2cRecvAck();

    i2cStart();
    i2cSendByte(SLAVE_ADDR | 0x01); i2cRecvAck();
    for (i = 0; i < len; i++) {
        buf[i] = i2cRecvByte();
        if (i < len - 1) i2cSendAck(0);
        else i2cSendAck(1);
    }
    i2cStop();
}

void mpu6050ReadAll(int *ax, int *ay, int *az, int *gx, int *gy, int *gz)
{
    uchar xdata buf[14];
    mpu6050ReadRegs(REG_ACCEL_XOUT_H, buf, 14);

    *ax  = ((int)buf[0]  << 8) | buf[1];
    *ay  = ((int)buf[2]  << 8) | buf[3];
    *az  = ((int)buf[4]  << 8) | buf[5];
    *gx  = ((int)buf[8]  << 8) | buf[9];
    *gy  = ((int)buf[10] << 8) | buf[11];
    *gz  = ((int)buf[12] << 8) | buf[13];
}

float xdata filterXZ = 0, filterYZ = 0, filterYX = 0;

void mpu6050Filter(int ax, int ay, int az, int gx, int gy, int gz,
                   float *roll, float *pitch, float *yaw)
{
    float accAngle;

    gx += GYRO_OFFSET_X;
    gy += GYRO_OFFSET_Y;
    gz += GYRO_OFFSET_Z;

    accAngle = -atan2(ax, az) / 3.1415926 * 180.0;
    filterXZ = FILTER_ALPHA * accAngle + (1.0 - FILTER_ALPHA) * (filterXZ + (float)gy / 32768.0 * 2000.0 * 0.025);

    accAngle = -atan2(ay, az) / 3.1415926 * 180.0;
    filterYZ = FILTER_ALPHA * accAngle + (1.0 - FILTER_ALPHA) * (filterYZ + (float)gx / 32768.0 * 2000.0 * 0.025);

    accAngle = -atan2(ay, ax) / 3.1415926 * 180.0;
    filterYX = FILTER_ALPHA * accAngle + (1.0 - FILTER_ALPHA) * (filterYX + (float)gz / 32768.0 * 2000.0 * 0.025);

    *roll  = filterXZ;
    *pitch = filterYZ;
    *yaw   = filterYX;
}
