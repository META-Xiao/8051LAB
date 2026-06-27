/**
 * ds18b20.h — DS18B20 数字温度传感器库
 *
 * 硬件: DQ = P0.2 (1-Wire)
 * 精度: 0.01°C (DS18B20 原生 12bit = 0.0625°C)
 */

#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "sysinit.h"

#ifndef _DS18B20_PIN_DEFINED_
sbit DQ = P0^2;
#define _DS18B20_PIN_DEFINED_
#endif

/* ===== API ===== */

/* 初始化引脚 */
void ds18b20Init(void);

/* 启动温度转换，立即返回。调用后需等待 >=750ms 再读 */
void ds18b20Start(void);

/* 读取温度，返回 temp×100 (如 3206 = 32.06°C)，需在 ds18b20Start 后 >=750ms 调用 */
uint ds18b20Read(void);

/* 一体式读取（阻塞约 800ms，内部调用 ds18b20Start + busy wait + ds18b20Read） */
uint ds18b20ReadTemp(void);

#endif
