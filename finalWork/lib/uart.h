/**
 * uart.h — UART0 串口通信库
 *
 * 默认 115200-8-N-1, TX=P0.4, RX=P0.5 (交叉开关自动分配)
 * 初始化后直接使用 stdio.h 的 printf / putchar 即可。
 */

#ifndef __UART_H__
#define __UART_H__

#include "sysinit.h"

/* ===== API ===== */

/* 默认初始化 (115200) */
void uartInit(void);

/* 指定波特率初始化，如 uartInitBaud(9600) */
void uartInitBaud(ulong baud);

#endif
