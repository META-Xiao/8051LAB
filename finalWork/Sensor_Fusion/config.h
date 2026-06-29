/**
 * config.h — sensorFusion 引脚配置
 *
 * sbit 必须在 sysinit.h 之后(需要 C8051F410.h 的 SFR 声明)
 * 但在对应库头文件之前(库内 #ifndef 守卫会跳过默认引脚)
 */
#include "sysinit.h"

/* ===== 超声波: TRIG=P0.6, ECHO=P0.7 ===== */
#define _ULTRA_PIN_DEFINED_
sbit TRIG = P0^6;
sbit ECHO = P0^7;

/* ===== DS18B20: DQ=P0.5 ===== */
#define _DS18B20_PIN_DEFINED_
sbit DQ = P0^5;

/* ===== PCF8591: SCL=P1.1, SDA=P1.0 ===== */
#define _PCF8591_PIN_DEFINED_
sbit PCF_SCL = P1^1;
sbit PCF_SDA = P1^0;

/* ===== 引入库 ===== */
#include "oled7pin.h"
#include "ultrasonic.h"
#include "ds18b20.h"
#include "pcf8591.h"
#include "buzzer.h"
#include "chart.h"
