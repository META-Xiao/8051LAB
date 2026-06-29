/* ESP8266 WIFI 驱动: UART透传 + AT指令引擎 */
#include "config.h"
#include "stdio.h"
#include "string.h"

static void espDelayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

static void espDelay10us(uchar n)
{
    uchar i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 18; j++);
    }
}

void esp8266Init(ulong baud)
{
    uartInitBaud(baud);
    ESP8266_RST = 1;
    espDelayMs(1000);
}

void esp8266HardReset(void)
{
    ESP8266_RST = 0;
    espDelayMs(200);
    ESP8266_RST = 1;
    espDelayMs(2000);
}

void esp8266SendByte(uchar dat)
{
    while (TI0 == 0);
    TI0 = 0;
    SBUF0 = dat;
}

void esp8266SendBuf(uchar *buf, uint len)
{
    uint i;
    for (i = 0; i < len; i++) {
        esp8266SendByte(buf[i]);
    }
}

uchar esp8266ReadByte(void)
{
    if (RI0) {
        RI0 = 0;
        return SBUF0;
    }
    return 0;
}

uchar esp8266Available(void)
{
    return RI0;
}

/**
 * 发送 AT 指令，在响应中搜索 "OK"，找到返回 1
 */
uchar esp8266SendAT(uchar *cmd, uint timeoutMs)
{
    uchar xdata buf[64];
    uint len, i;

    len = esp8266SendATResp(cmd, buf, sizeof(buf), timeoutMs);
    if (len == 0) return 0;

    for (i = 0; i < len - 1; i++) {
        if (buf[i] == 'O' && buf[i + 1] == 'K') return 1;
    }
    return 0;
}

/**
 * 发送 AT 指令并接收完整响应
 * 自动追加 "AT" 前缀和 \r\n，遇到 \r\nOK\r\n 或 \r\nERROR\r\n 时截断
 */
uint esp8266SendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs)
{
    uint idx = 0;
    uint elapsed = 0;

    esp8266SendByte('A');
    esp8266SendByte('T');
    if (cmd != 0 && cmd[0] != '\0') {
        esp8266SendBuf(cmd, strlen((char *)cmd));
    }
    esp8266SendByte('\r');
    esp8266SendByte('\n');

    while (elapsed < timeoutMs) {
        if (RI0) {
            RI0 = 0;
            if (idx < maxLen - 1) {
                buf[idx++] = SBUF0;
            }
            if (idx >= 6 &&
                buf[idx - 6] == '\r' && buf[idx - 5] == '\n' &&
                buf[idx - 4] == 'O'  && buf[idx - 3] == 'K' &&
                buf[idx - 2] == '\r' && buf[idx - 1] == '\n') {
                break;
            }
            if (idx >= 9 &&
                buf[idx - 9] == '\r' && buf[idx - 8] == '\n' &&
                buf[idx - 7] == 'E'  && buf[idx - 6] == 'R' &&
                buf[idx - 5] == 'R'  && buf[idx - 4] == 'O' &&
                buf[idx - 3] == 'R'  && buf[idx - 2] == '\r' &&
                buf[idx - 1] == '\n') {
                break;
            }
            elapsed = 0;
        }
        espDelayMs(1);
        elapsed++;
    }

    if (idx == 0) return 0;
    buf[idx] = '\0';
    return idx;
}

/**
 * 一键配置: STA模式 → 连WiFi → 连TCP → 开启透传
 * 任一步失败即返回 0, 全部成功返回 1
 */
uchar esp8266Config(WiFiConfig *cfg)
{
    if (!esp8266SetMode(1)) return 0;
    espDelayMs(500);
    if (!esp8266ConnectAP(cfg->ssid, cfg->password)) return 0;
    espDelayMs(1000);
    if (!esp8266ConnectTCP(cfg->serverIP, cfg->serverPort)) return 0;
    espDelayMs(500);
    if (!esp8266StartTrans()) return 0;
    espDelayMs(500);
    return 1;
}

uchar esp8266TestAT(void)
{
    return esp8266SendAT("", 2000);
}

/**
 * 设置工作模式 (1=STA,2=AP,3=STA+AP) 并重启生效
 */
uchar esp8266SetMode(uchar mode)
{
    uchar xdata cmd[12];
    sprintf((char *)cmd, "+CWMODE=%d", mode);
    if (!esp8266SendAT(cmd, 1000)) return 0;
    return esp8266SendAT("+RST", 3000);
}

/**
 * 连接 WiFi 热点, 成功后自动设 CIPMUX=0, 重试 5 次
 */
uchar esp8266ConnectAP(uchar *ssid, uchar *pwd)
{
    uchar xdata cmd[60];
    uint retry = 0;

    espDelayMs(2000);
    sprintf((char *)cmd, "+CWJAP=\"%s\",\"%s\"", (char *)ssid, (char *)pwd);

    while (retry < 5) {
        if (esp8266SendAT(cmd, 8000)) {
            if (esp8266SendAT("+CIPMUX=0", 1000)) return 1;
        }
        retry++;
        espDelayMs(1000);
    }
    return 0;
}

/**
 * 建立 TCP 连接, ip 为字符串, port 为端口号
 */
uchar esp8266ConnectTCP(uchar *ip, uint port)
{
    uchar xdata cmd[50];
    sprintf((char *)cmd, "+CIPSTART=\"TCP\",\"%s\",%d", (char *)ip, port);
    return esp8266SendAT(cmd, 10000);
}

/**
 * 开启透传 (AT+CIPMODE=1 → AT+CIPSEND)
 * 成功后 printf 全部发往 TCP 服务器
 */
uchar esp8266StartTrans(void)
{
    if (!esp8266SendAT("+CIPMODE=1", 1000)) return 0;
    espDelayMs(200);
    return esp8266SendAT("+CIPSEND", 1000);
}

/**
 * 退出透传: 空闲 1s → 发送 +++ → 等待 1s
 */
void esp8266ExitTrans(void)
{
    espDelayMs(1000);
    esp8266SendByte('+');
    esp8266SendByte('+');
    esp8266SendByte('+');
    espDelayMs(1000);
}

void esp8266SendTrans(uchar *buf, uint len)
{
    esp8266SendBuf(buf, len);
}

uchar esp8266ResetFactory(void)
{
    return esp8266SendAT("+RESTORE", 3000);
}

/**
 * 查询当前是否已连接 AP, 响应含 +CWJAP: 且无 FAIL 标志则返回 1
 */
uchar esp8266CheckConnected(void)
{
    uchar xdata buf[32];
    uint len, i;

    len = esp8266SendATResp("+CWJAP?", buf, sizeof(buf), 3000);
    if (len == 0) return 0;

    for (i = 0; i < len - 6; i++) {
        if (buf[i] == '+' && buf[i + 1] == 'C' &&
            buf[i + 2] == 'W' && buf[i + 3] == 'J' &&
            buf[i + 4] == 'A' && buf[i + 5] == 'P') {
            if (i + 30 < len) {
                uchar j;
                for (j = i; j < i + 30; j++) {
                    if (buf[j] == 'F' && buf[j + 1] == 'A' &&
                        buf[j + 2] == 'I' && buf[j + 3] == 'L') {
                        return 0;
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}
