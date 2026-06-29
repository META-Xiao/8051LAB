#include "config.h"
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
		
    printf("=====TEST======\r \n");
	
    while (1) {
        printf("RAW:%d,%d,%d | %d,%d,%d | F:%d,%d,%d\r\n", AX, AY, AZ, GX, GY, GZ, (int)roll, (int)pitch, (int)yaw);
        delayMs(30);
    }
}

void t2ISR() interrupt 5
{
    TF2H = 0;
    mpu6050ReadAll(&AX, &AY, &AZ, &GX, &GY, &GZ);
    mpu6050Filter(AX, AY, AZ, GX, GY, GZ, &roll, &pitch, &yaw);
}
