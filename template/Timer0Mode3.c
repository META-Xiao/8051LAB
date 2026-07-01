#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

// 注意该定时器模式只有T0具有
// 目的是将T0变成两个8位的定时器
// 低位溢出标志就是TF0，高位溢出标志就是TF1
// 同时T1的启动位和中断使能位都被TH0抢走
// TR1就是TH0的启动位，ET1就是TH0的中断使能位

// 并且此时的TL0可以作为定时器/计数器
// 但是TH0只能作为定时器

// T1本身由于没有启动位和中断使能位，虽然可以工作在MODE0/1/2

// 如果T0配置了MODE3，启动和停止由TMOD模式位控制
// 具体TMOD控制是TMOD=0/1/2时，T1自动运行不受TR1控制
// 当TMOD=3时，T1停止运行，此时如果之前是计时器方式，那么可以读取TH1和TL1

// 这里T1如果是计数模式呢？TF1还是可以读的，但是是查询的方式
// 如果是使用中断，也就是TF1溢出时JAMP，此时由于001BH这个中断地址被TH0占用
// 并且此时TF1被TH0占用，001BH实际相应的也必然是TH0的溢出
// T1此时无法使用中断

// 通常作为UART的波特率发生器

void init(uint8 us0, uint8 us1)
{
    uint8 c0=us0*(fosc/1000000UL)/12;
    uint8 c1=us1*(fosc/1000000UL)/12;
    uint8 load0=0x100-c0; // 256-c;
    uint8 load1=0x100-c1;

    TMOD=0x03; // T0M3
    TH0=load1;
    TL0=load0;
    
    // 此时ET1控制TH0的中断使能位  ET0控制TL0的中断使能位
    // 此时TR1控制TH0的启动位  TR0控制TL0的启动位
    EA=1;
    ET0=1;
    ET1=1;
    TR0=1;
    TR1=1;

    TH1=256-(fosc/(384UL*9600UL));
    TL1=TH1;
    SCON = 0x50; // Mode1 REN=1 (允许接收
    TMOD |= 0x20; // T1M2 此时设定为波特率发生器 并启动
    TI = 1;
}

