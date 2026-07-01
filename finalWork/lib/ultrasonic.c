/**
 * HC-SR04 原理:
 *   Trig >= 10us 触发 → 模块发 8 个 40kHz 脉冲
 *   → Echo 输出高电平，持续 = 超声波往返时间
 *   → distance(cm) = Echo_time(us) / 58.8
 *
 * T0 时钟 = sysclk/12 = 24.5MHz/12 ≈ 2.0417MHz
 * 1 tick ≈ 0.49us
 * distance = ticks * 0.49 / 58.8 = ticks / 120
 */

#include "config.h"
#include "intrins.h"

static void ultraDelayUs(uint us)
{
    uchar i;
    while (us--) {
        for (i = 0; i < 4; i++) {
            _nop_();
        }
    }
}

void ultraInit(void)
{
    _ULTRA_CFG();
    TRIG = 0;
}

uint ultraRead(void)
{
    uint ticks;
    bit savedEA;

    TH0 = 0x00;
    TL0 = 0x00;
    TR0 = 0;

    TRIG = 1;
    ultraDelayUs(15);
    TRIG = 0;

    savedEA = EA;
    EA = 0;


    // 这里使用轮询方式等待 ECHO 信号
    // 就是每次测量距离会一卡一卡的根源
    // 优化方案就是使用GATE模式+INT0/1中断方式测量
    while (!ECHO);
    TR0 = 1; // 反复开关TR防止ECHO被其他定时器冲突
    while (ECHO);
    TR0 = 0;

    EA = savedEA;

    ticks = (TH0 << 8) | TL0;
    return (uint)((ulong)ticks / 120UL);
}
