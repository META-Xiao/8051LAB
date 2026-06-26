/**
 * main.c — BLE 5.0 蓝牙透传: MPU6050 → 手机
 *
 * 硬件 (仅4线):
 *   BLE VCC → 3.3V    BLE GND → GND
 *   BLE TXD → P0.5    BLE RXD → P0.4
 *   MPU6050 SDA=P0.2  SCL=P0.3
 *
 * 手机端: BLE调试助手 → 连接 → 启用Notify → 收数据
 */

#include "sysinit.h"
#include "bluetooth.h"
#include "mpu6050.h"
#include "stdio.h"
#include "math.h"

int   xdata AX, AY, AZ, GX, GY, GZ;
float xdata roll, pitch, yaw;

static void delayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

static void initT2(void)
{
    TMR2CN   = 0x04;
    TMR2RLL  = 0xF9;
    TMR2RLH  = 0x38;
    TMR2L    = 0xF9;
    TMR2H    = 0x38;
    ET2      = 1;
}

void main(void)
{
    sysInit();
    btInit(115200);
    mpu6050Init();
    initT2();

    printf("=== BLE MPU6050 ===\r\n");

    while (1) {
        printf("A:%d,%d,%d G:%d,%d,%d R%d,P%d,Y%d\r\n",
               AX, AY, AZ, GX, GY, GZ,
               (int)roll, (int)pitch, (int)yaw);
        delayMs(100);
    }
}

void t2ISR(void) interrupt 5
{
    TF2H = 0;
    mpu6050ReadAll(&AX, &AY, &AZ, &GX, &GY, &GZ);
    mpu6050Filter(AX, AY, AZ, GX, GY, GZ, &roll, &pitch, &yaw);
}
