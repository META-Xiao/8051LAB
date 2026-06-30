#include <reg51.h>

#define fosc 12000000UL

typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

sbit OUT = P1^0;

// 查询
static void out(uint16 us)
{
    uint16 cnt=us*(fosc/1000000UL)/12;
    uint8 load=0x100-cnt;

    TMOD=0x02;
    TH0=load;
    TL0=load;
    TF0=0;
    TR0=1;

    while(1)
    {
        if(TF0==1)
        {
            TF0=0;
            OUT = ~OUT;
        }
        // 以上语句等效于
        // while(!TF0);
        // OUT = ~OUT;
        // TF0=0;
    }

}


// 中断
// 输入高电平/低电平时间
void init(uint16 us)
{
    uint16 cnt=us*(fosc/1000000UL)/12;
    uint8 load=0x100-cnt; // 256-cnt;

    TMOD=0x02; //T0 (M1 M0)=2
    TH0=load;
    TL0=load;
    TF0=0;
    TR0=1;
    ET0=1;
    EA=1;
}

void isr(void) interrupt 1
{
    OUT = ~OUT;
}


