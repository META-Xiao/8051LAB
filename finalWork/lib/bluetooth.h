/* BLE 5.0 / HC-05 蓝牙模块库, UART透传 + AT指令 */
#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "sysinit.h"

sbit BT_KEY = P0^1;

typedef struct {
    uchar  role;
    ulong  baud;
    uchar *name;
    uchar *pincode;
} BtConfig;

void btInit(ulong baud);
void btInitBaud(ulong baud);
void btSendByte(uchar dat);
void btSendBuf(uchar *buf, uint len);
void btEnterATMode(void);
void btExitATMode(void);
uchar btSendAT(uchar *cmd, uint timeoutMs);
uint btSendATResp(uchar *cmd, uchar *buf, uint maxLen, uint timeoutMs);
uchar btConfig(BtConfig *cfg);
uchar btResetFactory(void);
uchar btReadByte(void);
uchar btAvailable(void);

#define btSendStr(s)  printf(s)

#endif
