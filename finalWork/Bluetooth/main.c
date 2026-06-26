/* BLE 5.0 透传: MPU6050 -> 手机
   VCC-3.3V  GND-GND  TXD-P0.5  RXD-P0.4
   MPU6050: SDA-P0.2  SCL-P0.3 */
#include "sysinit.h"
#include "uart.h"
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
    uartInit();
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
