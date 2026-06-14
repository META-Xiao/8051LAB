/**
 * sysinit.h — C8051F410 系统初始化
 *
 * 使用: 在 main.c 中 #include "sysinit.h"，调用 sysInit() 即可。
 *
 * 用户需在 main.c 中自行 sbit 的外设引脚:
 *   Trig  → 超声波触发  (例: sbit Trig = P0^0;)
 *   Echo  → 超声波回波  (例: sbit Echo = P0^1;)
 *   以及其他业务相关的 I/O
 *
 * 数码管和蜂鸣器引脚已分别由 segdisplay.h / buzzer.h 内部定义，无需再写。
 */

#ifndef __SYSINIT_H__
#define __SYSINIT_H__

#include "C8051F410.h"

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/* ===== 一次性初始化（标准场景） ===== */
void sysInit(void);

/* ===== 分模块初始化（按需调用） ===== */

/* 内部振荡器 24.5 MHz */
void sysOscInit(void);

/* 禁止看门狗 (PCA) */
void sysPcaInit(void);

/* 定时器: T0=门控16位, T1=自动重装8位, 波特率由 TH1 决定 */
void sysTimerInit(void);

/* 交叉开关: UART0 TX/RX 连到 P0.4/P0.5，其他外设跳过 */
void sysPortInit(void);

/* 中断: 开总中断 + Timer1 */
void sysIntInit(void);

#endif
