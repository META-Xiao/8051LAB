/* 片内IDAC0 锯齿波发生器
   P0.0 输出, 0-2mA 对应 0-4095 阶梯上升 */
#include "sysinit.h"
#include "uart.h"
#include "dac.h"
#include "stdio.h"

void main(void)
{
    uint i;

    sysInit();
    uartInit();
    dacInit();

    printf("\r\n=== IDAC Sawtooth ===\r\n");

    while (1) {
        for (i = 0; i < 4096; i++) {
            dacSet(i);
        }
    }
}
