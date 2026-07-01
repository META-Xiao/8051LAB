#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

uint16 load;

// 最大65.5ms
bool delay(uint16 ms)
{
    uint16 time=ms*1000;
    uint16 cnt=time*(fosc/1000000UL)/12;
    load=65536UL-cnt;

    TMOD=0x10;   //0001 0000 等效于 M0=1
    TH1=(uint8)(load>>8);
    TL1=(uint8)load;
    TF1=0;
    TR1=1;
    while(!TF1);
    TR1=0;
    TF1=0;
    return 1;
}


// 如果使用中断的话

void init_isr(uint16 ms)
{
    uint16 time=ms*1000;
    uint16 cnt=time*(fosc/1000000UL)/12;
    load=65536UL-cnt;

    TMOD=0x10;   //0001 0000
    TH1=(uint8)(load>>8);
    TL1=(uint8)load;
    EA=1; // 总中断使能开启
    ET1=1; // Timer1中断使能开启
    TF1=0;
    TR1=1;
}

// 注意
// 使用中断没有自动重载会导致赋值时MOV指令消耗时间
// 导致每次都会延迟一点时间
// 如果是PWM这样要求准确的信号
// 那么推荐还是使用定时器自动重载method=2

void delay_isr(void) interrupt 3
{
    TH1=(uint8)(load>>8);
    TL1=(uint8)load;
    // TF1=0; 其中断自动清零，没必要手动再写一遍
    // 这里写要做的任务
    // 比如
    // if(f)P1=0x01;
    // else P1=0x00;
}

// 当然timer还可以用于计数
// 这里写个最简单的例子

static uint32 counter()
{
    uint32 res=0;
    TMOD=0x50; // 0101 0000 开启Counter模式以及模式1
    TH1=0;
    TL1=0;
    TR1=1;
    // 等待函数结束
    while(!XXXisEnd())
    {
        if(TF1)
        {
            res+=65536UL;
            TF1=0, TH1=0, TL1=0;
        }
    }
    TR1=0;
    res+=((uint16)(TH1<<8) | TL1);
    return res;
    // return (node){TL1, TH1};
}


// 如果是中断的话
// 直接把每次溢出加个3中断就行
