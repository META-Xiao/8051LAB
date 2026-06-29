/* C8051F410 片内12位ADC驱动*/
#include "config.h"

void adcInit(void)
{
    REF0CN  = 0x0A;
    P0MDIN  = 0xF9;
    P0SKIP  = 0x06;
    ADC0MX  = 0x02;
    ADC0CF  = 0x38;
    ADC0CN  = 0x80;
}

uint adcRead(uchar channel)
{
    ADC0MX  = channel & 0x1F;
    AD0INT  = 0;
    AD0BUSY = 1;
    while (AD0BUSY);
    return ADC0;
}

uint adcToMv(uint raw)
{
    return (uint)(((ulong)raw * 2500UL) / 4096UL);
}
