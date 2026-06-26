/**
 * bluetooth.h — HC-05 蓝牙模块库 (蓝牙5.0 亚博BLE模组)
 *
 * 硬件连接 (UART 透传):
 *   HC-05 VCC → MCU VCC
 *   HC-05 GND → MCU GND
 *   HC-05 TXD → MCU RXD (P0.5)
 *   HC-05 RXD → MCU TXD (P0.4)
 *   HC-05 KEY → MCU P0^1 (AT模式, 可选)
 *
 * 工作模式:
 *   - 数据透传模式 (默认): 串口数据直接无线传输到配对手机
 *   - AT指令模式: 用于配置模块参数 (名称/波特率/角色等)
 *
 * 典型波特率: 9600 (默认), 38400, 115200
 *
 * 使用示例:
 *   btInit(9600);                          // UART初始化 + 蓝牙配对上电
 *   printf("hello\r\n");                   // 直接通过蓝牙发送到手机
 *   btSendStr("Temperature: 25.0C\r\n");  // 等价于 printf
 */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "sysinit.h"

/* HC-05 KEY 引脚 (进入AT模式用, 可选引脚) */
sbit BT_KEY = P0^1;

/* ===== 蓝牙模块配置结构体 ===== */
typedef struct {
    uchar  role;      /* 0=从机, 1=主机, 2=回环 (默认0)  */
    ulong  baud;      /* 波特率 (默认9600)                */
    uchar *name;      /* 设备名称 (最长31字符)            */
    uchar *pincode;   /* 配对码   (默认"1234")            */
} BtConfig;

/* ===== 基本 API ===== */

/* 初始化蓝牙模块:
 *   - 初始化 UART0 并配置指定波特率
 *   - 可选: 拉高 KEY 进入 AT 模式进行参数配置
 *   调用后即可用 printf / btSendStr 发送数据到手机
 */
void btInit(ulong baud);

/* 仅初始化UART波特率 (不改变蓝牙模块的AT配置) */
void btInitBaud(ulong baud);

/* 发送一个字节 */
void btSendByte(uchar dat);

/* 发送字节数组 */
void btSendBuf(uchar *buf, uint len);

/* ===== AT 指令 API (高级配置用) ===== */

/* 进入AT模式 (拉高KEY复位模块 / 发送时序) */
void btEnterATMode(void);

/* 退出AT模式 (恢复透传) */
void btExitATMode(void);

/* 发送一条AT指令并等待应答, 返回1=OK, 0=超时/错误
 *   cmd:  AT指令内容 (不含"AT"前缀, 如 "NAME=MyBT" 或 "" 仅测试)
 *   timeoutMs: 等待超时 (典型1000ms)
 */
uchar btSendAT(uchar *cmd, uint timeoutMs);

/* 发送AT指令并获取完整应答字符串
 *   buf:  接收缓冲区
 *   maxLen: 缓冲区大小
 *   timeoutMs: 超时
 *   返回应答长度, 0=超时
 */
uint btSendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs);

/* ===== 批量配置 API ===== */

/* 根据 BtConfig 一键配置模块, 返回 1=成功
 *   调用前会自动进入AT模式, 完成后退出
 */
uchar btConfig(BtConfig *cfg);

/* 快速恢复出厂设置 */
uchar btResetFactory(void);

/* ===== 数据透传辅助 ===== */

/* 发送字符串 (等价于 printf, 用于语义清晰) */
#define btSendStr(s)  printf(s)

/* 接收一个字节 (非阻塞, 无数据返回0) */
uchar btReadByte(void);

/* 检查是否有数据到达 */
uchar btAvailable(void);

#endif
