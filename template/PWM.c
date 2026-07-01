#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

// 这里假定方波周期为1ms
// 输入整数的占空比0-100(%)


// 这里默认使用中断
// 中断准确点

sbit OUT = P1^0;
uint8 cnt=0;
uint8 pwm=50;



// 10us分频
// 这里1%的1ms就是10us
// 所以高电平就是pwm*10us
void init()
{
    uint16 c=10UL*(fosc/1000000UL)/12;
    uint8 load=0x100-c; // 256-c;

    TMOD = 0x20; // 定时器1，工作模式2
    TH1 = load;
    TL1 = load;
    TF1 = 0;
    TR1 = 1;
    EA = 1;
    ET1 = 1;
    
}

void pwm_isr() interrupt 3
{
    cnt++;
    if(cnt>=1000) cnt=0;

    OUT=(cnt<pwm)? 1:0;
}

// 值得注意的是以上的PWM在中断由于本身就有1~4us的相应延迟
// 所以占空比还是会有一定的误差1%左右


// 同样对于周期为10ms的PWM
// 1%的10ms就是100us
// 改成uint16 c=100UL*(fosc/1000000UL)/12; 即可

// 但是8位最大是256
// 如果使用100ms的周期，那么1%的100ms就是1ms
// 此时如果设置一个溢出周期1ms的话
// 就是下面这样

// void pwm_isr() interrupt 3
// {
//     cnt0++;
//     if(cnt0>=100)
//     {
//         cnt0=0; cnt++;
//         if(cnt>=100) cnt=0;
//     }
//     OUT=(cnt<pwm)? 1: 0;
// }

