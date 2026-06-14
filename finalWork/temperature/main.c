#include "sysinit.h"
#include "uart.h"
#include "ds18b20.h"
#include "segdisplay.h"
#include "buzzer.h"
#include "stdio.h"

void t2ISR() interrupt 5
{
    TF2H = 0;
    buzzerTick();
}

void main(void)
{
    uint temp;
    uint lastTemp = 0xFFFF;
    uchar x;

    sysInit();
    uartInit();
    ds18b20Init();
    segInit();
    buzzerInit();
    buzzerSetupTimer();

    printf("=== DS18B20 Ready ===\r\n");

    while (1) {
        ds18b20Start();

        for (x = 0; x < 200; x++) {
            segShowRaw(0, segCode(lastTemp / 1000));
            segShowRaw(1, segCode(((lastTemp / 100) % 10)) & 0x7F);
            segShowRaw(2, segCode((lastTemp / 10) % 10));
            segShowRaw(3, segCode(lastTemp % 10));
        }

        temp = ds18b20Read();

        if (temp != lastTemp) {
            printf("temp = %u.%02u C\r\n", temp / 100, temp % 100);
            lastTemp = temp;
        }

        if (temp >= 3300) buzzerStart(1000, 80, 100, 3);
    }
}
