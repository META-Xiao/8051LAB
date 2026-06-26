/**
 * bluetooth.c — HC-05 / BLE 5.0 蓝牙模块驱动
 *
 * 基于 UART0 透传, 默认波特率 9600 (匹配 HC-05 出厂值)
 * TX=P0.4, RX=P0.5 (交叉开关自动分配)
 */

#include "bluetooth.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

/* 内部延时 (24.5MHz 系统时钟) */
static void btDelayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

/* 延时 10us 级 (用于 AT 模式时序) */
static void btDelay10us(uchar n)
{
    uchar i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 18; j++);
    }
}

/* ===== 基本 API ===== */

void btInit(ulong baud)
{
    /* 初始化 UART0 为指定波特率 */
    uartInitBaud(baud);

    /* BLE 5.0: MOD=高=透传模式 (与HC-05相反!) */
    BT_KEY = 1;

    /* 等待蓝牙模块稳定上电 */
    btDelayMs(500);
}

void btInitBaud(ulong baud)
{
    uartInitBaud(baud);
}

void btSendByte(uchar dat)
{
    SBUF0 = dat;
    while (TI0 == 0);
    TI0 = 0;
}

void btSendBuf(uchar *buf, uint len)
{
    uint i;
    for (i = 0; i < len; i++) {
        btSendByte(buf[i]);
    }
}

/* ===== AT 指令 API ===== */

void btEnterATMode(void)
{
    /* BLE 5.0: MOD=低=AT指令模式 (与HC-05相反!) */
    BT_KEY = 0;
    btDelayMs(100);
}

void btExitATMode(void)
{
    /* BLE 5.0: MOD=高=透传模式 */
    BT_KEY = 1;
    btDelayMs(100);
}

uchar btSendAT(uchar *cmd, uint timeoutMs)
{
    uchar xdata buf[32];
    uint len;

    len = btSendATResp(cmd, buf, sizeof(buf), timeoutMs);

    if (len == 0) return 0;

    /* 检查应答中是否包含 "OK" */
    {
        uint i;
        for (i = 0; i < len - 1; i++) {
            if (buf[i] == 'O' && buf[i + 1] == 'K') return 1;
        }
    }
    return 0;
}

uint btSendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs)
{
    uint idx = 0;
    uint elapsed = 0;

    /* 发送 "AT" + 指令 + "\r\n" */
    btSendStr("AT");
    if (cmd != 0 && cmd[0] != '\0') {
        btSendBuf(cmd, strlen((char *)cmd));
    }
    btSendByte('\r');
    btSendByte('\n');

    /* 等待应答, 超时返回0 */
    while (elapsed < timeoutMs) {
        if (RI0) {
            RI0 = 0;
            if (idx < maxLen - 1) {
                buf[idx++] = SBUF0;
            }
            /* 检测 "\r\nOK\r\n" 或 "\r\nERROR\r\n" 终止符
             * 简化判断: 收到 OK 或 ERROR 后不再等待新数据
             */
            if (idx >= 4) {
                if (buf[idx - 4] == '\r' && buf[idx - 3] == '\n' &&
                    buf[idx - 2] == 'O'  && buf[idx - 1] == 'K') {
                    break;
                }
                if (idx >= 8 &&
                    buf[idx - 8] == '\r' && buf[idx - 7] == '\n' &&
                    buf[idx - 6] == 'E'  && buf[idx - 5] == 'R' &&
                    buf[idx - 4] == 'R'  && buf[idx - 3] == 'O' &&
                    buf[idx - 2] == 'R') {
                    break;
                }
            }
            elapsed = 0;  /* 有数据到达, 重置超时 */
        }
        btDelayMs(1);
        elapsed++;
    }

    if (idx == 0) return 0;

    buf[idx] = '\0';
    return idx;
}

/* ===== 批量配置 API ===== */

uchar btConfig(BtConfig *cfg)
{
    uchar ok;

    btEnterATMode();

    /* 基础连接测试 */
    ok = btSendAT("", 1000);
    if (!ok) {
        btExitATMode();
        return 0;
    }

    /* 设置角色 */
    if (cfg->role <= 2) {
        btSendAT("+ROLE=0", 1000);  /* 默认从机 */
    }

    /* 设置设备名称 */
    if (cfg->name != 0 && cfg->name[0] != '\0') {
        uchar xdata cmd[40];
        sprintf((char *)cmd, "+NAME=%s", (char *)cfg->name);
        btSendAT(cmd, 1000);
    }

    /* 设置配对码 */
    if (cfg->pincode != 0 && cfg->pincode[0] != '\0') {
        uchar xdata cmd[20];
        sprintf((char *)cmd, "+PSWD=%s", (char *)cfg->pincode);
        btSendAT(cmd, 1000);
    }

    /* 设置波特率 (HC-05: 1=1200,2=2400,3=4800,4=9600,5=19200,6=38400,7=57600,8=115200) */
    if (cfg->baud > 0) {
        uchar baudCode;
        switch (cfg->baud) {
            case 1200:   baudCode = 1; break;
            case 2400:   baudCode = 2; break;
            case 4800:   baudCode = 3; break;
            case 9600:   baudCode = 4; break;
            case 19200:  baudCode = 5; break;
            case 38400:  baudCode = 6; break;
            case 57600:  baudCode = 7; break;
            case 115200: baudCode = 8; break;
            default:     baudCode = 4; break;  /* 默认 9600 */
        }
        {
            uchar xdata cmd[15];
            sprintf((char *)cmd, "+UART=%d,0,0", baudCode);
            btSendAT(cmd, 1000);
        }
    }

    btExitATMode();
    return 1;
}

uchar btResetFactory(void)
{
    uchar ok;

    btEnterATMode();
    ok = btSendAT("+ORGL", 3000);  /* 恢复出厂设置, 需要更长超时 */
    btExitATMode();

    return ok;
}

/* ===== 数据接收 ===== */

uchar btReadByte(void)
{
    if (RI0) {
        RI0 = 0;
        return SBUF0;
    }
    return 0;
}

uchar btAvailable(void)
{
    return RI0;
}
