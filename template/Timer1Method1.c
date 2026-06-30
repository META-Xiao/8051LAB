#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

uint16 load;

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

void init_isr(uint ms)
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

