/**
 * keyboard.c — 4×4 矩阵键盘扫描实现
 *
 * 行列反转扫描法:
 *   1. 行输出低电平, 列输入(上拉), 读列状态
 *   2. 列输出低电平, 行输入(上拉), 读行状态
 *   3. 组合行列编码 → 查表得键号 1~16
 */

#include "config.h"
#include "intrins.h"

/* ===== 4×4 键盘行列编码表 ===== */
static uchar code key_code[16] = {
    0xEE, 0xDE, 0xBE, 0x7E,   /* 第0行: 键1 ~ 键4  */
    0xED, 0xDD, 0xBD, 0x7D,   /* 第1行: 键5 ~ 键8  */
    0xEB, 0xDB, 0xBB, 0x7B,   /* 第2行: 键9 ~ 键12 */
    0xE7, 0xD7, 0xB7, 0x77    /* 第3行: 键13 ~ 键16 */
};

/* ===== 内部延时 ===== */
static void keyDelayMs(uint ms)
{
    uchar i, j;
    while (ms--) {
        for (i = 0; i < 25; i++) {
            for (j = 0; j < 250; j++) {
                _nop_();
            }
        }
    }
}

/* ===== 初始化键盘端口 ===== */
void keyboardInit(void)
{
    P0MDIN =0xFF, P0MDOUT=0x00;
    P0=0xFF;   /* P0 全部拉高 (关断, 等待输入) */
}

/* ===== 查表===== */
uchar keyboardDecode(uchar keycode)
{
    uchar j;
    for (j = 0; j <= 15; j++)
    {
        if(keycode==key_code[j]) return j + 1;
    }
    return 0;
}

/* ===== 阻塞式行列反转扫描 (保留兼容) ===== */
uchar keyscan(void)
{
    uchar scan1, scan2, keycode, j;
    P0=0xF0;
    scan1=P0;
    if((scan1 & 0xF0) != 0xF0)
    {
        keyDelayMs(20);
        scan1=P0;
        if((scan1 & 0xF0) != 0xF0)   /* 二次确认 */
        {
            P0=0x0F, scan2=P0;
            keycode = scan1 | scan2;   /* 组合行列编码 */
            for(j = 0; j <= 15; j++)
            {
                if (keycode == key_code[j])
                    return j+1;
            }
        }
    }
    return KEY_NONE;
}
