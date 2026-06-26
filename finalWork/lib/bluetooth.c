/* BLE 5.0 / HC-05 蓝牙驱动: UART透传 + AT指令引擎 */
#include "bluetooth.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

static void btDelayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++);
    }
}

static void btDelay10us(uchar n)
{
    uchar i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 18; j++);
    }
}

void btInit(ulong baud)
{
    uartInitBaud(baud);
    BT_KEY = 1;
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

void btEnterATMode(void)
{
    BT_KEY = 0;
    btDelayMs(100);
}

void btExitATMode(void)
{
    BT_KEY = 1;
    btDelayMs(100);
}

uchar btSendAT(uchar *cmd, uint timeoutMs)
{
    uchar xdata buf[32];
    uint len, i;

    len = btSendATResp(cmd, buf, sizeof(buf), timeoutMs);
    if (len == 0) return 0;

    for (i = 0; i < len - 1; i++) {
        if (buf[i] == 'O' && buf[i + 1] == 'K') return 1;
    }
    return 0;
}

uint btSendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs)
{
    uint idx = 0;
    uint elapsed = 0;

    btSendStr("AT");
    if (cmd != 0 && cmd[0] != '\0') {
        btSendBuf(cmd, strlen((char *)cmd));
    }
    btSendByte('\r');
    btSendByte('\n');

    while (elapsed < timeoutMs) {
        if (RI0) {
            RI0 = 0;
            if (idx < maxLen - 1) {
                buf[idx++] = SBUF0;
            }
            if (idx >= 4 &&
                buf[idx - 4] == '\r' && buf[idx - 3] == '\n' &&
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
            elapsed = 0;
        }
        btDelayMs(1);
        elapsed++;
    }

    if (idx == 0) return 0;
    buf[idx] = '\0';
    return idx;
}

uchar btConfig(BtConfig *cfg)
{
    if (!btSendAT("", 1000)) return 0;

    if (cfg->name != 0 && cfg->name[0] != '\0') {
        uchar xdata cmd[40];
        sprintf((char *)cmd, "+NAME=%s", (char *)cfg->name);
        btSendAT(cmd, 1000);
    }

    if (cfg->pincode != 0 && cfg->pincode[0] != '\0') {
        uchar xdata cmd[20];
        sprintf((char *)cmd, "+PSWD=%s", (char *)cfg->pincode);
        btSendAT(cmd, 1000);
    }

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
            default:     baudCode = 4; break;
        }
        {
            uchar xdata cmd[15];
            sprintf((char *)cmd, "+UART=%d,0,0", baudCode);
            btSendAT(cmd, 1000);
        }
    }

    return 1;
}

uchar btResetFactory(void)
{
    return btSendAT("+ORGL", 3000);
}

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
