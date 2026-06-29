/**
 * segdisplay.h — 数码管显示库
 *
 * 硬件: P2=段选(共阴,高亮), P1.0~P1.3=位选(低使能,千→个)
 */

#ifndef __SEGDISPLAY_H__
#define __SEGDISPLAY_H__

#include "sysinit.h"

/* ===== 硬件映射 ===== */
#ifndef _SEG_PIN_DEFINED_
#define SEG_PORT        P2
sbit SEG_DIGIT0 = P1^0;
sbit SEG_DIGIT1 = P1^1;
sbit SEG_DIGIT2 = P1^2;
sbit SEG_DIGIT3 = P1^3;
#define _SEG_PIN_DEFINED_
#endif
#define SEG_DIGIT_COUNT 4

/* ===== API ===== */
void segInit(void);
void segShow(uint num);
void segShowHex(uint num);
void segShowBuf(uchar *bufp, uchar len);
void segShowRaw(uchar pos, uchar seg);
void segScan(void);
void segClear(void);
uchar segCode(uchar digit);
uchar segHexCode(uchar val);
void segDelayMs(uint ms);

#endif
