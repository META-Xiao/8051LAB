/* 片内ADC数字电压表
   模拟输入: P0.2, 量程 0-2.5V */
#include "config.h"
#include "stdio.h"

void main(void)
{
    uint adcVal, mV;
    uint lastAdc = 0xFFFF;
    uchar i;

    sysInit();
    uartInit();
    adcInit();
    segInit();

    printf("\r\n=== ADC Voltmeter ===\r\n");

    while (1) {
        adcVal = adcRead(2);
        mV = adcToMv(adcVal);

        if (adcVal != lastAdc) {
            printf("P0.2: %4u (%u.%03uV)\r\n",
                   adcVal, mV / 1000, mV % 1000);
            lastAdc = adcVal;
        }

        for (i = 0; i < 50; i++) {
            segShowRaw(0, segCode(mV / 1000) & 0x7F);
            segShowRaw(1, segCode((mV / 100) % 10));
            segShowRaw(2, segCode((mV / 10) % 10));
            segShowRaw(3, segCode(mV % 10));
        }
    }
}
