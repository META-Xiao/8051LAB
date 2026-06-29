/**
 * mpu6050.h — MPU6050 6轴传感器库
 *
 * 硬件: SDA=P0.2, SCL=P0.3 (P0.3 推挽, P0.2 开漏)
 */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "sysinit.h"

#ifndef _MPU6050_PIN_DEFINED_
sbit SDA = P0^2;
sbit SCL = P0^3;
#define _MPU6050_PIN_DEFINED_
#endif

/* 互补滤波参数 (按需修改) */
#define FILTER_ALPHA    0.001
#define GYRO_OFFSET_X   19
#define GYRO_OFFSET_Y   43
#define GYRO_OFFSET_Z   13

/* ===== API ===== */

void mpu6050Init(void);
void mpu6050ReadRegs(uchar reg, uchar *buf, uchar len);
void mpu6050ReadAll(int *ax, int *ay, int *az, int *gx, int *gy, int *gz);

/* 互补滤波: 输入6轴原始值, 输出3轴姿态角(度) */
void mpu6050Filter(int ax, int ay, int az, int gx, int gy, int gz,
                   float *roll, float *pitch, float *yaw);

#endif
