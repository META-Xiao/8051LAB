/**
 * chart.c — OLED 柱状曲线图 (帧缓冲+原子传输+跳帧)
 */
#include "chart.h"
#include "oled7pin.h"

#define BUF_SIZE 64
#define MAX_COL  128

static uchar xdata fb[MAX_COL * 7];    /* 帧缓冲 XDATA */
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

/* 帧缓冲画一列 */
static void fbCol(uchar x, uchar valPx)
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
        fb[p * MAX_COL + x] = mask;
    }
}

static void fbClear(void)
{
    uint i;
    for (i = 0; i < (uint)pages * MAX_COL; i++) fb[i] = 0;
}

/* 帧缓冲→OLED (关中断, 尽量原子) */
static void fbFlush(void)
{
    uchar p, savedEA;
    uint  x;
    savedEA = EA;  EA = 0;          /* 传输期间关中断防撕裂 */
    for (p = 0; p < pages; p++) {
        oledWriteByte(0xB0 + y0 + p, 0);
        oledWriteByte(0x00, 0);
        oledWriteByte(0x10, 0);
        for (x = 0; x < MAX_COL; x++)
            oledWriteByte(fb[p * MAX_COL + x], 1);
    }
    EA = savedEA;
}

static void chartDraw(void)
{
    uchar i;
    /* 不 fbClear! 帧缓冲保留上一帧, 新柱直接覆盖 */
    for (i = 0; i < BUF_SIZE; i++) {
        uchar v   = buf[(idx + i) & (BUF_SIZE - 1)];
        uchar bar = (yMax > 0) ? (uchar)((uint)v * pxH / yMax) : 0;
        if (bar > pxH) bar = pxH;
        fbCol(i * 2,     bar);
        fbCol(i * 2 + 1, bar);
    }
    fbFlush();
}

void chartReset(uchar newMax)
{
    uchar i;
    idx  = 0;
    yMax = newMax ? newMax : 1;
    for (i = 0; i < BUF_SIZE; i++) buf[i] = 0;
    fbClear();
    fbFlush();
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
