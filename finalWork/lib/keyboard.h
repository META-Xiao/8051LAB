/**
 * keyboard.h — 4×4 矩阵键盘扫描库
 *
 * 硬件: P0.0~P0.3 → 行线 (Row)
 *       P0.4~P0.7 → 列线 (Col)
 * 方法: 行列反转扫描法
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "sysinit.h"

#define KEY_NONE  0xFF   /* 无按键按下时返回此值 */

/* ===== API ===== */

/* 初始化 P0 端口为键盘扫描模式 (开漏 + 数字输入) */
void keyboardInit(void);

/* 阻塞式扫描键盘，返回键号 (1~16)，无按键返回 KEY_NONE */
uchar keyscan(void);

/* 根据行列组合编码查表，返回键号 (1~16)，无效返回 0 */
uchar keyboardDecode(uchar keycode);

#endif
