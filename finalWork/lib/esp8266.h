/* ESP8266 WIFI 模块库: UART透传 + AT指令
   VCC-5V  GND-GND  RXD-P0.4  TXD-P0.5  RST-P0.7
   参考: https://yngzmiao.blog.csdn.net/article/details/86538288 */
#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "sysinit.h"

sbit ESP8266_RST = P0^7;

typedef struct {
    uchar *ssid;
    uchar *password;
    uchar *serverIP;
    uint   serverPort;
} WiFiConfig;

void esp8266Init(ulong baud);
void esp8266HardReset(void);

void esp8266SendByte(uchar dat);
void esp8266SendBuf(uchar *buf, uint len);
uchar esp8266ReadByte(void);
uchar esp8266Available(void);

/**
 * 发送 AT 指令，检测 OK 后返回 1，超时返回 0
 * cmd: 不含 "AT" 前缀和 \r\n 的指令体 (空串 = 仅发 AT)
 */
uchar esp8266SendAT(uchar *cmd, uint timeoutMs);

/**
 * 发送 AT 指令，返回完整响应字符串 (含 \0 结尾)
 * 返回值: 接收字节数 (不含 \0), 0 = 超时
 */
uint esp8266SendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs);

/**
 * 一键配置: STA模式 → 连WiFi → 连TCP → 开启透传
 * 任一步失败即返回 0
 */
uchar esp8266Config(WiFiConfig *cfg);

uchar esp8266TestAT(void);
uchar esp8266SetMode(uchar mode);
uchar esp8266ConnectAP(uchar *ssid, uchar *pwd);
uchar esp8266ConnectTCP(uchar *ip, uint port);

/**
 * 开启透传模式 (AT+CIPMODE=1 + AT+CIPSEND)
 * 成功后所有 printf 直接发往 TCP 服务器
 */
uchar esp8266StartTrans(void);

/** 退出透传 (发送 +++) */
void esp8266ExitTrans(void);

/** 透传下发送二进制数据 (printf 亦可) */
void esp8266SendTrans(uchar *buf, uint len);

uchar esp8266ResetFactory(void);

/** 查询是否已连上 AP, 1=已连 */
uchar esp8266CheckConnected(void);

#endif
