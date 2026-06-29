#include "config.h"
#include "stdio.h"

void t2ISR() interrupt 5
{
    TF2H = 0;
    buzzerTick();
}

void main(void)
{
    uint dist;
    uint lastDist = 0;
    uchar x;

    sysInit();
    uartInit();
    buzzerInit();
    buzzerSetupTimer();
    ultraInit();
    segInit();

    printf("=== Ranging Ready ===\r\n");

    while(1)
    {
        dist = ultraRead();
        if (dist != lastDist)
				{
            printf("dist = %u cm\r\n", dist);
            lastDist = dist;
        }
        if (dist<20) buzzerStart(2000, 50, 20, 5);
        else if (dist<50) buzzerStart(1000, 80, 100, 3);
        else if (dist<100) buzzerStart(800, 50, 300, 2);

        for (x = 0; x < 50; x++) {
            segShowRaw(0, segCode(dist / 1000));
            segShowRaw(1, segCode((dist / 100) % 10));
            segShowRaw(2, segCode((dist / 10) % 10));
            segShowRaw(3, segCode(dist % 10));
        }
    }
}
