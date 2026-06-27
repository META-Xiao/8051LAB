/**
 * oled7pin.h — 0.96" OLED SSD1306 7pin SPI 驱动
 *
 * 硬件: SCL=P0.0  SDA=P0.1  RST=P0.2  DC=P0.3  CS=P0.4
 * 协议: 4线SPI, 仅写, 128×64
 */
#ifndef __OLED7PIN_H__
#define __OLED7PIN_H__

#include "sysinit.h"

/* ===== 类型 ===== */
#define OLED_CMD  0
#define OLED_DATA 1

/* ===== 引脚定义 ===== */
sbit OLED_SCL = P0^0;
sbit OLED_SDA = P0^1;
sbit OLED_RST = P0^2;
sbit OLED_DC  = P0^3;
sbit OLED_CS  = P0^4;

/* ===== 宏 ===== */
#define OLED_SCL_CLR()  OLED_SCL = 0
#define OLED_SCL_SET()  OLED_SCL = 1
#define OLED_SDA_CLR()  OLED_SDA = 0
#define OLED_SDA_SET()  OLED_SDA = 1
#define OLED_RST_CLR()  OLED_RST = 0
#define OLED_RST_SET()  OLED_RST = 1
#define OLED_DC_CLR()   OLED_DC  = 0
#define OLED_DC_SET()   OLED_DC  = 1
#define OLED_CS_CLR()   OLED_CS  = 0
#define OLED_CS_SET()   OLED_CS  = 1

#define OLED_COL_MAX 128
#define OLED_ROW_MAX 64
#define OLED_FONT_W  8
#define OLED_FONT_H  16

/* ===== API ===== */

/** 初始化 SSD1306, 清屏 */
void oledInit(void);

/** 清屏(全黑) */
void oledClear(void);

/** 开/关显示 */
void oledDisplayOn(void);
void oledDisplayOff(void);

/**
 * 在 (x,y) 显示 ASCII 字符 chr (8×16)
 * x: 0~127, y: 0~6 (页), 自动换行
 */
void oledWriteByte(uchar dat, uchar cmd);
void oledShowChar(uchar x, uchar y, uchar chr);

/**
 * 在 (x,y) 显示 ASCII 字符串, 以 '\0' 结尾
 * 超出宽度自动换行
 */
void oledShowStr(uchar x, uchar y, uchar *str);

/**
 * 在 (x,y) 显示中文字符, no 为 Hzk 数组中的序号
 * 每个汉字占 16×16(宽16), 占两行(同页+下一页)
 */
void oledShowCn(uchar x, uchar y, uchar no);

/**
 * 显示 Hzk 中的窄字符(8×16), 只写有效8字节, 无浪费空白
 * 间距用8px即可, 密度比 oledShowCn 高一倍
 */
void oledShowChar8(uchar x, uchar y, uchar no);

/**
 * 显示数字 num(0~4294967295), 左对齐
 * len: 位数, 不足前补空格
 * size: 字号 (16=大, 12=小)
 */
void oledShowNum(uchar x, uchar y, ulong num, uchar len, uchar size);

/** 画 BMP 位图, x0/y0 起始, x1/y1 尺寸(像素/页) */
void oledDrawBMP(uchar x0, uchar y0, uchar x1, uchar y1, uchar code *bmp);

/** 软件延时 ms */
void oledDelayMs(uint ms);

#endif
