/* PCF8591 数字电压表
   I2C: P0.3(SCL) P0.2(SDA), AIN1 模拟输入
*/
#include "config.h"
#include "stdio.h"

void main(void)
{
    uint val;
    uint mV;
    uint lastVal = 0xFFFF;
    uchar i;

    sysInit();
    uartInit();
    segInit();
    pcf8591Init();

    printf("\r\n=== PCF8591 Voltmeter ===\r\n");

    while (1) {
        val = pcf8591ReadADC(1);
        mV = (uint)((ulong)val * 5000UL / 255UL);

        if (val != lastVal) {
            printf("AIN1: %3u (%u.%03uV)\r\n",
                   val, mV / 1000, mV % 1000);
            lastVal = val;
        }

        for (i = 0; i < 50; i++) {
            segShowRaw(0, segCode(mV / 1000)& 0x7F);
            segShowRaw(1, segCode((mV / 100) % 10));
            segShowRaw(2, segCode((mV / 10) % 10));
            segShowRaw(3, segCode(mV % 10));
        }
    }
}
