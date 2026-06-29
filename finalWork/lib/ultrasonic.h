/**
 * ultrasonic.h — HC-SR04 超声波测距库
 *
 * 硬件: Trig=P1.0, Echo=P1.1
 * Timer0 用于脉宽测量（sysTimerInit 已配置模式1 16位）
 */

#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "sysinit.h"

#ifndef _ULTRA_PIN_DEFINED_
sbit TRIG = P0^0;
sbit ECHO = P0^1;
#define _ULTRA_PIN_DEFINED_
#endif

/* 端口配置: TRIG=推挽, ECHO=输入上拉 */
#ifndef _ULTRA_CFG
#define _ULTRA_CFG() do { \
    P0MDOUT |=  0x01;      \
    P0MDOUT &= ~0x02;      \
    P0MDIN  |=  0x02;      \
} while(0)
#endif

/* ===== API ===== */

/* 初始化引脚，Trig 拉低 */
void ultraInit(void);

/* 触发一次测距，返回距离 (cm)，阻塞约 0.1~30ms（取决于距离） */
uint ultraRead(void);

#endif
