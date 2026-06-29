#include "config.h"

void sysPcaInit(void)
{
    PCA0MD &= ~0x40;
    PCA0MD  = 0x00;
}

void sysTimerInit(void)
{
    TCON  = 0x50;
    TMOD  = 0x11;
}

void sysPortInit(void)
{
    XBR1 = 0x40;
}

void sysOscInit(void)
{
    OSCICN = 0x87;
}

void sysIntInit(void)
{
    IE = 0x80;
}

void sysInit(void)
{
    sysPcaInit();
    sysTimerInit();
    sysPortInit();
    sysOscInit();
    sysIntInit();
}
