/* 
   C8051F410 片内12位电流型IDAC
   IDAC0 → P0.0, 输出范围 0-2mA 
   需要短接J16(DA0)
*/
#ifndef __DAC_H__
#define __DAC_H__

#include "sysinit.h"

/* 端口配置: P0.0 模拟输入 */
#ifndef _DAC_CFG
#define _DAC_CFG() do { \
    P0MDIN &= ~0x01;      \
    P0SKIP |=  0x01;      \
} while(0)
#endif

#define DAC_SINE_POINTS 256

/**
 * 初始化 IDAC0: 2mA量程, P0.0 模拟输出
 */
void dacInit(void);

/**
 * 设置 IDAC0 输出值 (12bit, 0-4095 → 0-2mA)
 */
void dacSet(uint value);

/** 正弦波码表 (256点, code 区) */
extern const uint code dacSineTable[DAC_SINE_POINTS];

#endif
