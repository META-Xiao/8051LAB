/* 
C8051F410 片内12位ADC 
需要短接J16(DA0)
*/
#ifndef __ADC_H__
#define __ADC_H__

#include "sysinit.h"

/**
 * 初始化 ADC: 内部2.5V基准, P0.0/P0.1 模拟输入, SAR=2.7MHz
 */
void adcInit(void);

/**
 * 读取指定通道 ADC 值 (12bit, 0-4095)
 * channel: 0-26 (对应 ADC0MX)
 */
uint adcRead(uchar channel);

/**
 * 将 ADC 原始值转为 mV (基于 2.5V 基准)
 */
uint adcToMv(uint raw);

#endif
