/**
 * ultrasonic.h — HC-SR04 超声波测距库
 *
 * 硬件: Trig=P1.0, Echo=P1.1
 * Timer0 用于脉宽测量（sysTimerInit 已配置模式1 16位）
 */

#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "sysinit.h"

sbit TRIG = P0^0;
sbit ECHO = P0^1;

/* ===== API ===== */

/* 初始化引脚，Trig 拉低 */
void ultraInit(void);

/* 触发一次测距，返回距离 (cm)，阻塞约 0.1~30ms（取决于距离） */
uint ultraRead(void);

#endif
