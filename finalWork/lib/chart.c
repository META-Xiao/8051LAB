/**
 * chart.c — OLED 实时曲线图
 *
 * 在指定页面区域绘制 64 点柱状曲线, 128×N px, 自适应 Y 轴
 */
#include "chart.h"
#include "oled7pin.h"

#define BUF_SIZE 64

static uchar xdata buf[BUF_SIZE];
static uchar idx, y0, pages, pxH, yMax;

static uchar code barMask[9] = {
    0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF
};

void chartInit(uchar startPage, uchar numPages, uchar initMax)
{
    uchar i;
    y0    = startPage;
    pages = numPages;
    pxH   = numPages * 8;
    yMax  = initMax ? initMax : 1;
    idx   = 0;
    for (i = 0; i < BUF_SIZE; i++) buf[i] = 0;
}

/** 清空图表区域 */
static void chartClear(void)
{
    uchar y, x;
    for (y = 0; y < pages; y++) {
        oledWriteByte(0xB0 + y0 + y, 0);  /* set page */
        oledWriteByte(0x00, 0);            /* col low */
        oledWriteByte(0x10, 0);            /* col high */
        for (x = 0; x < 128; x++)
            oledWriteByte(0x00, 1);
    }
}

/** 画一列: x 位置, 高 valPx 像素(0=底, pxH=满) */
static void chartCol(uchar x, uchar valPx)
{
    uchar p;
    int   barTop;

    barTop = (int)(y0 + pages) * 8 - (int)valPx;

    for (p = 0; p < pages; p++) {
        int   pageTop = (int)(y0 + p) * 8;
        int   pageBot = pageTop + 7;
        uchar mask;

        if (barTop > pageBot)      mask = 0x00;
        else if (barTop <= pageTop) mask = 0xFF;
        else                       mask = barMask[pageBot - barTop + 1];

        oledWriteByte(0xB0 + y0 + p, 0);          /* 每页独立设地址 */
        oledWriteByte(((x & 0xF0) >> 4) | 0x10, 0);
        oledWriteByte((x & 0x0F) | 0x01, 0);
        oledWriteByte(mask, 1);
    }
}

/** 重绘全部曲线 */
static void chartDraw(void)
{
    uchar i;
    chartClear();
    for (i = 0; i < BUF_SIZE; i++) {
        uchar v   = buf[(idx + i) & (BUF_SIZE - 1)];
        uchar bar = (yMax > 0) ? (uchar)((uint)v * pxH / yMax) : 0;
        if (bar > pxH) bar = pxH;
        chartCol(i * 2,     bar);  /* 每bar 2px宽 */
        chartCol(i * 2 + 1, bar);
    }
}

void chartReset(uchar newMax)
{
    uchar i;
    idx  = 0;
    yMax = newMax ? newMax : 1;
    for (i = 0; i < BUF_SIZE; i++) buf[i] = 0;
    chartClear();
}

void chartPush(uchar val)
{
    uchar i;
    buf[idx] = val;
    idx = (idx + 1) & (BUF_SIZE - 1);

    yMax = 0;
    for (i = 0; i < BUF_SIZE; i++)
        if (buf[i] > yMax) yMax = buf[i];
    if (yMax == 0) yMax = 1;

    chartDraw();
}
