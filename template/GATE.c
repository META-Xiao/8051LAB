/*
本文件是TIMER里面的GATE使用样例
GATE=1后，其启动由INT0/INT1控制
INT0/INT1为高电平时，TIMER才会启动
其为低电平时，TIMER停止
就是为了方便测量外部的脉冲宽度
重点是只测量单个脉冲宽度，不能测量连续脉冲宽度
例如
1. HC-SR04 的 Echo 引脚输出：触发后，高电平持续时间 = 声波往返时间
2. NEC 红外协议中，引导码的高电平9ms 低电平4.5ms；
    数据位中，0是0.56ms高 + 0.565ms低 
            1是0.56ms高 + 1.69ms低
    此时就需要精确测量每个高电平的宽度来判断是引导码还是数据位
3. 某个外部设备输出一个忙信号，高电平期间表示正在处理
    欲要测量这次处理到底花了多长时间，GATE 直接给出
以上如果使用while在主程序中轮询等待ECHO高电平结束
    测量时就会过度占用CPU，导致此时无法处理其他任务

*/
#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

/*
INT0	P3.2	外部中断 0，也是 T0 的 GATE 输入
INT1	P3.3	外部中断 1，也是 T1 的 GATE 输入
*/

// 查询方式见finalWork\lib\ultrasonic.c
// 中断方式要求信号是单次脉冲或周期足够长
// 大于中断处理时间，网上测量出来的是1~4us


// 这里外部改变的寄存器最好使用 volatile 修饰
volatile uint16 pulseWidth=0; // 记录脉冲宽度
volatile bit pulseEnd=0; // 一个脉冲结束标志

void init()
{
    // INT0 下降沿触发
    // 具体是下降沿进入中断表示一次测量结束
    IT0=1;  EX0=1;  EA=1;
    TMOD=0x09; // T0开启GATE和模式1

    while(INT0); // 等待 INT0 因为上电时触发的上升沿过去
    // 确保INT0是低 第一次开启T0
    TH0=0, TL0=0;
    TR0=1;   
}

void Int0Isr() interrupt 0
{
    // 下降沿进入中断
    // 此时 T0 已自动停止
    pulseEnd = 1; // 脉冲结束
    pulseWidth = (uint16)((TH0 << 8) | TL0);
}

// 这个当上次测量结束，即pulseEnd=1时，重置T0
// 但是T0由于开启了GATE
// 只有当INT0引脚变为高电平时，T0才会启动计时
void startNxt()
{
    pulseEnd=0;
    TH0=0, TL0=0;
    TR0=1;    
}

void main()
{
    init();
    uint16 t0=12000000UL/fosc;
    while(1)
    {
        if(pulseEnd)
        {
            startNxt();
        }
        uint16 pulseTime=pulseWidth*t0;
    }
}

