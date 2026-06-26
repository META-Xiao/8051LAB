/* UART0 串口: 自动适配 T1M=0/1, 覆盖 1200~115200 */
#include "uart.h"
#include "stdio.h"

#define SYS_CLK  24500000UL

static ulong sysClk = SYS_CLK;

void uartInit(void)
{
    uartInitBaud(115200);
}

void uartInitBaud(ulong baud)
{
    ulong divisor;
    uchar reload;

    XBR0 |= 0x01;
    XBR1 |= 0x40;

    TMOD = (TMOD & 0x0F) | 0x20;

    divisor = sysClk / (2UL * baud);
    if (divisor > 0 && divisor <= 256) {
        CKCON |= 0x08;
        reload = (uchar)(256UL - divisor);
    } else {
        CKCON &= ~0x08;
        divisor = sysClk / (24UL * baud);
        if (divisor > 256 || divisor == 0) {
            divisor = 256;
        }
        reload = (uchar)(256UL - divisor);
    }

    TH1 = reload;
    SCON0 = 0x10;
    TR1 = 1;
    TI0 = 1;
}

char putchar(char c)
{
    while (TI0 == 0);
    TI0 = 0;
    SBUF0 = c;
    return c;
}
