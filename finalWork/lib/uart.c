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
    uchar reload;

    XBR0 |= 0x01;
    XBR1 |= 0x40;

    CKCON |= 0x08;

    TMOD = (TMOD & 0x0F) | 0x20;

    reload = (uchar)(256UL - sysClk / (2UL * baud));
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
