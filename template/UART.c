#include <reg51.h>
#include <stdio.h>

#define fosc 11059200UL
#define baud 9600UL
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;


void uartInit()
{
    TMOD |= 0x20;       // T1M2  这里做或是为了不影响其他定时器的配置
                        // 实际上如果确定只设置这个
                        // 也可以直接写 TMOD = 0x20
    TH1=256-(fosc/(384UL*baud));
    TL1=TH1;
    TR1=1;
    SCON = 0x50;        // Mode1 REN=1 (允许接收)
                        // 万精油记住就行
    TI = 1;             // 设置发送就绪标志，让第一个 putchar 不阻塞
}


// 配置好UART后直接就可以使用printf和scanf
// 原本的printf和scanf是输出到屏幕和从键盘输入
// keil编译环境重定向这些到串口了
void main()
{
    uartInit();
    char buf[16];
    int num;

    while(1)
    {
        printf("enter string: ");
        scanf("%s", buf);            // 读字符串
        printf("you: %s\r\n", buf);

        printf("enter number: ");
        scanf("%d", &num);           // 读整数
        printf("num: %d\r\n", num);
    }
}

// 当然这个嵌入式UART很有意思的是
// 在PC上gcc的读入逻辑也可以用在这里
// 例如 scanf("%c", &c); if(c=='\n' || c=='\r') break; 直接跳出阅读
// 但是不能使用EOF '\0' 之类的结束符










// ============ 以下函数没必要再次重写 ============
// keil的编译环境重载了这些函数

// ================= 轮询方式 =================
// 重写putchar
// 重定向到串口
char putchar(char c)
{
    // 这里加个自动换行的功能
    if(c=='\n') 
    {
        while(!TI); 
        TI=0;
        SBUF='\r';
    }


    while(!TI);       // 等待上一字节发完
    TI=0;            // 清发送标志
    SBUF=c;          // 发送当前字节
    return c;
}

// 重定向 scanf / getchar 的底层输入函数 (注意函数名是 _getkey)
char _getkey(void)
{
    char c;
    while (!RI);       // 阻塞等待接收完成标志
    RI=0;            // 清接收标志
    c=SBUF;          // 读取数据
    return c;
}

// ================= 中断方式 =================
// !!!
// 这里必须开始serial（串口）中断 ES=1
// 以及确保总中断开启 EA=1
// !!!
// 在实际的嵌入式开发中
// 接收数据的时机是未知的
// 轮询接收会把 CPU 卡死在 _getkey 里
// 导致单片机无法处理其他硬件逻辑
// 因此标准的做法是
// 接收用中断、发送用轮询

// 因为发送是你主动发起的
// 稍微等一下通常没关系
// 如果连发送也不想等，那就需要写基于环形队列的纯异步发送了
// 对于简单的 printf 来说有些杀鸡用牛刀😅
// 至少8051上没必要
// 如果需要则自行研究下高速串口发送的环形队列实现

// volatile 告诉编译器别偷懒
// 每次都要真的去读内存
// 被外部修改的寄存器（变量）必须加 volatile
// 否则编译器的优化会让你的代码看不见这些变化
volatile uint8 recvData=0;
volatile bit flag=0;

void uartIsr() interrupt 4
{
    if(RI)  // 先确定是不是接收中断
    {
        RI=0;
        recvData=SBUF;
        flag=1;
    }
    // if(TI) TI=0;
    // 如果使用中断发送，这里处理TI。
    // 但为了配合下面的轮询 putchar
    // 我们这里不对 TI 做处理
}

// 重定向 scanf / getchar 的底层输入函数
char _getkey(void)
{
    char c;
    while(!flag);  // 等待中断服务函数把标志位置1
    flag=0;       // 清除标志，为下一次接收做准备
    c=recvData;       // 取出数据
    return c;
}

// 发送依然保持轮询方式
char putchar(char c)
{
    if(c=='\n') 
    {
        while(!TI); 
        TI=0;
        SBUF='\r';
    }

    while(!TI);
    TI=0;
    SBUF=c;
    return c;
}



// 实际上这个接收也是keil自带的重定向getchar函数
// 也可以直接使用
// uint8 c=getchar();
