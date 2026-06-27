/**
 * KeyBoard — 4×4 矩阵键盘 P2 + UART 调试 P0
 *
 * 硬件: P2 → 4×4 矩阵键盘 (P2.0~3行, P2.4~7列)
 *       P0.4/P0.5 → UART TX/RX
 *       P1.0~P1.3 → 数码管位选, P2 → 段选(已挪走)
 *
 * 注意: 键盘搬到 P2 避免和 UART(P0) 冲突
 */
#include "sysinit.h"
#include "uart.h"
#include "stdio.h"

/* 简单延时 */
static void kbdDelay(void)
{
    uint i;
    for (i = 0; i < 6000; i++);
}

/* ===== 键盘 P2 引脚 ===== */
#define KBD_PORT  P2

/* 行列编码表 */
static uchar code keyCode[16] = {
    0x77,0xB7,0xD7,0xE7, 0x7B,0xBB,0xDB,0xEB,
    0x7D,0xBD,0xDD,0xED, 0x7E,0xBE,0xDE,0xEE
};

/** 查表: 编码 → 键号 1~16, 无效返回 0 */
uchar kbdDecode(uchar kc)
{
    uchar j;
    for (j = 0; j < 16; j++)
        if (kc == keyCode[j]) return j + 1;
    return 0;
}

/** 行列反转扫描, 返回键号 1~16, 无键返回 0 */
uchar kbdScan(void)
{
    uchar s1, s2, kc;

    KBD_PORT = 0xF0;
    s1 = KBD_PORT;
    if ((s1 & 0xF0) == 0xF0) return 0;

    kbdDelay();                      /* 去抖 */

    KBD_PORT = 0xF0;
    s1 = KBD_PORT;
    if ((s1 & 0xF0) == 0xF0) return 0;

    KBD_PORT = 0x0F;
    s2 = KBD_PORT;
    kc = s1 | s2;
    return kbdDecode(kc);
}

void main(void)
{
    uchar key, lastKey = 0;

    sysInit();
    uartInit();
    P2MDOUT = 0x0F;
    P2      = 0xFF;

    printf("\r\n=== Keyboard Debug (P2) ===\r\n");

    while (1)
    {
        key = kbdScan();

        if (key && key != lastKey)
        {
            printf("Key: %2u\r\n", (uint)key);
            lastKey = key;
        }
        if (!key) lastKey = 0;
    }
}
