/* 片内IDAC0 正弦波发生器
   P0.0 输出, 256点查表 0-4095 */
#include "config.h"
#include "stdio.h"

void main(void)
{
    uint i;

    sysInit();
    uartInit();
    dacInit();

    printf("\r\n=== IDAC Sine Wave ===\r\n");

    while (1) {
        for (i = 0; i < DAC_SINE_POINTS; i++) {
            dacSet(dacSineTable[i]);
        }
    }
}
