/* PCF8591 AD/DA 模块, I2C 接口
   SCL=P0.3  SDA=P0.2  地址 0x90 */
#ifndef __PCF8591_H__
#define __PCF8591_H__

#include "sysinit.h"

#ifndef _PCF8591_PIN_DEFINED_
sbit PCF_SCL = P0^3;
sbit PCF_SDA = P0^2;
#define _PCF8591_PIN_DEFINED_
#endif

/**
 * 初始化 I2C 引脚 (开漏 + 上拉)
 */
void pcf8591Init(void);

/**
 * 读取 AIN0-3 通道 ADC 值 (8bit, 0-255)
 */
uchar pcf8591ReadADC(uchar channel);

/**
 * 输出 DAC 值 (8bit, 0-255 → 0-Vref)
 */
void pcf8591WriteDAC(uchar value);

/**
 * 读取所有4通道, 存入 buf[4]
 */
void pcf8591ReadAll(uchar *buf);

#endif
