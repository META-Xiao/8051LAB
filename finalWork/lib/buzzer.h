/**
 * buzzer.h — 蜂鸣器驱动库
 *
 * 硬件: P1.7 → 蜂鸣器
 *   阻塞模式: buzzerBeep / buzzerBeepPattern（软件延时，简单但占 CPU）
 *   非阻塞:   buzzerStart / buzzerTick（需 T1 中断，与显示并行）
 */

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "sysinit.h"

#ifndef _BUZZER_PIN_DEFINED_
sbit BUZZER_PIN = P1^7;
#define _BUZZER_PIN_DEFINED_
#endif

/* ===== 阻塞 API（简单场景） ===== */
void buzzerInit(void);
void buzzerBeep(uint freqHz, uint durMs);
void buzzerBeepPattern(uint freqHz, uint onMs, uint offMs, uchar cycles);
void buzzerOn(void);
void buzzerOff(void);
void buzzerToggle(void);

/* ===== 非阻塞 ===== */

/* 配置 T2 为蜂鸣器时基 (~250us/次)，并使能 ET2 */
void buzzerSetupTimer(void);

/* 启动非阻塞鸣叫模式，立即返回；主循环继续扫屏不受影响 */
void buzzerStart(uint freqHz, uint onMs, uint offMs, uchar cycles);

/* 停止当前鸣叫 */
void buzzerStop(void);

/* 是否正在鸣叫中 */
uchar buzzerBusy(void);

/* 放 T1 ISR 中调用，管理鸣叫状态机 */
void buzzerTick(void);

#endif
