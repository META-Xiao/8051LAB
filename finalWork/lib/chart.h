/**
 * chart.h — OLED 实时曲线图库
 *
 * 在指定 OLED 页面区域绘制柱状曲线, 支持自动缩放 Y 轴
 *
 * chartInit(y0, pages, maxVal):
 *   y0=起始页(0~7), pages=占用页数, maxVal=Y轴初始上限(0=自适应)
 * chartPush(val): 推入新数据, 自动重绘曲线
 */
#ifndef __CHART_H__
#define __CHART_H__

#include "sysinit.h"

void chartInit(uchar y0, uchar pages, uchar maxVal);
void chartPush(uchar val);
void chartReset(uchar maxVal);   /* 清空曲线, 设新上限 */

#endif
