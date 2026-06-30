# 8051LAB — MCS-51 寄存器参考 & 代码模板

图表由 `gen_8051_charts.py` 和 `gen_8051_overview.py` 生成（需要 `matplotlib`）。

---

## 寄存器图表

### SFR 全览

![SFR Overview](charts/sfr_overview_master.png)

标准 8051 全部 SFR 一览，每行一个寄存器，8 个 bit 逐位展开。绿色格子 = 可位寻址，蓝色 = 不可位寻址，紫色 = 8052 专属。格子下方灰色数字为位地址。最右列为复位值。

### 内存空间全景

![Memory Organization](charts/memory_overview.png)

哈佛架构的四个独立地址空间：程序存储器 (ROM) 左侧带中断向量表红色高亮，内部 RAM 低 128 字节含寄存器组和位寻址区，高 128 字节的 SFR 区与间接寻址 RAM (8052)，右侧为外部数据存储器 (XRAM)。

### 内部 RAM 低 128 字节

![RAM Low 128](charts/ram_low128.png)

0x00 到 0x7F 逐格色块。寄存器组 Bank 0-3、位寻址区 (0x20-0x2F)、通用数据区 (0x30-0x7F)。底部红色提醒：SP 复位值为 0x07，第一个 PUSH 会写入 0x08（覆盖 Bank 1 的 R0），须在 `main()` 中 `SP = 0x60`。

### ROM 中断向量表

![ROM IVT](charts/rom_ivt.png)

程序存储器最低 0x33 字节。每个中断占 8 字节槽位，通常前 3 字节放 `LJMP` 指令跳转到实际 ISR。下方青色区域为 0x0033 开始的用户程序区。

### 位寻址区

![Bit Addressable](charts/bit_addressable.png)

16 字节 x 8 位 = 128 个独立可寻址位 (0x00-0x7F)。位地址 = N x 8 + b。C 语言中用 `uchar bdata flags; sbit LED = flags^3;` 由编译器自动分配。

### SFR 地址映射

![SFR Map](charts/sfr_map.png)

0x80-0xFF 共 128 个地址槽。只有地址末位为 0 或 8 的 SFR 才可位寻址。灰色为保留/未使用。

### IE 中断使能寄存器

![IE](charts/ie_register.png)

EA 是总开关。每位控制对应中断源的使能 (1=开, 0=关)。ET2 仅 8052 有效。

```c
IE = 0x80;  // 仅开总中断
IE = 0x82;  // EA + Timer 0
IE = 0x90;  // EA + Timer 1 + 串口
IE = 0x81;  // EA + INT0
```

### IP 中断优先级寄存器

![IP](charts/ip_register.png)

仅两级优先级：0 (低) 和 1 (高)。高优先级 ISR 可打断低优先级。同级中断同时触发时按自然优先级排队。

### 中断自然优先级

![Interrupt Priority](charts/interrupt_priority.png)

硬件每机器周期按此固定顺序轮询。INT0 始终最先检查。各向量地址间隔 8 字节。

### TCON 定时器控制寄存器

![TCON](charts/tcon_register.png)

高 4 位控制 Timer 1，低 4 位控制 Timer 0，同时包含外部中断触发方式选择。

```c
TCON = 0x50;  // TR1=1, TR0=1  同时启动两个定时器
TCON = 0x40;  // 仅启动 T1 (UART 波特率)
TCON = 0x05;  // IT0=IT1=1  双外部中断下降沿触发
```

### TMOD 定时器模式寄存器

![TMOD](charts/tmod_register.png)

不可位寻址，必须整字节写入。高半字节配置 Timer 1，低半字节配置 Timer 0。每半字节：GATE | C/T | M1 | M0。

### 定时器四种工作模式

![Timer Modes](charts/timer_modes.png)

| 模式 | M1:M0 | 位数 | 自动重装 | 典型用途 |
|:---|:---:|:---|:---|:---|
| 0 | 00 | 13 位 | 否 | 兼容 8048，极少使用 |
| 1 | 01 | 16 位 | 否 | 精确计时，ISR 中手动重装 |
| 2 | 10 | 8 位 | 是 (TH->TL) | UART 波特率发生器 |
| 3 | 11 | 双 8 位 (仅 T0) | 否 | 需要三个定时器时 |

```c
TMOD = 0x01;  // T0 模式1, T1 模式0
TMOD = 0x11;  // 双 16 位
TMOD = 0x20;  // T1 模式2 (UART 波特率)
```

### SCON 串行口控制寄存器

![SCON](charts/scon_register.png)

TI 和 RI 必须软件清零——硬件只置位不清零。

```c
SCON = 0x50;  // 模式1 (8位UART), REN=1  标准初始化
```

### Keil C51 存储器类型

![Memory Types](charts/memory_types.png)

---

## 代码模板

所有模板使用标准 8051 SFR 名称。修改 `FOSC` 匹配实际晶振频率。

### Timer 0 模式 1（16 位，手动重装）

传入微秒数，根据 FOSC 自动计算初值。Mode 1 下 TL0 存完整的低 8 位（不是 Mode 0 的低 5 位）。

```c
#ifndef FOSC
#define FOSC 12000000UL
#endif

// --- 查询方式 ---
void Timer0_Mode1_Init(unsigned int us)
{
    unsigned long count  = (unsigned long)us * (FOSC / 1000000UL) / 12;
    unsigned int  reload = 65536UL - count;

    TMOD = 0x01;             // T0 模式 1 (16 位)
    TH0  = (unsigned char)(reload >> 8);     // 高 8 位
    TL0  = (unsigned char)(reload);          // 低 8 位 (Mode1 用满 8 bit)
    TF0  = 0;
    TR0  = 1;
}

void Timer0_Wait(void)
{
    while (TF0 == 0);
    TF0 = 0;
    TR0 = 0;
}

// --- 中断方式 ---
unsigned int t0_reload;

void Timer0_Mode1_Init_IT(unsigned int us)
{
    unsigned long count  = (unsigned long)us * (FOSC / 1000000UL) / 12;
    t0_reload = 65536UL - count;

    TMOD = 0x01;
    TH0  = (unsigned char)(t0_reload >> 8);
    TL0  = (unsigned char)(t0_reload);
    ET0  = 1;
    EA   = 1;
    TR0  = 1;
}

void Timer0_ISR(void) interrupt 1
{
    TH0 = (unsigned char)(t0_reload >> 8);
    TL0 = (unsigned char)(t0_reload);
    // 用户代码
}
```

### Timer 1 模式 1（16 位，手动重装）

与 Timer 0 用法相同，只是 `TMOD = 0x10`、寄存器改为 TH1/TL1/TF1/TR1，中断号为 `interrupt 3`。

```c
void Timer1_Mode1_Init(unsigned int us)
{
    unsigned long count  = (unsigned long)us * (FOSC / 1000000UL) / 12;
    unsigned int  reload = 65536UL - count;

    TMOD = 0x10;             // T1 模式 1 (16 位)
    TH1  = (unsigned char)(reload >> 8);
    TL1  = (unsigned char)(reload);
    TF1  = 0;
    TR1  = 1;
}

void Timer1_Wait(void)
{
    while (TF1 == 0);
    TF1 = 0;
    TR1 = 0;
}

unsigned int t1_reload;

void Timer1_Mode1_Init_IT(unsigned int us)
{
    unsigned long count  = (unsigned long)us * (FOSC / 1000000UL) / 12;
    t1_reload = 65536UL - count;

    TMOD = 0x10;
    TH1  = (unsigned char)(t1_reload >> 8);
    TL1  = (unsigned char)(t1_reload);
    ET1  = 1;
    EA   = 1;
    TR1  = 1;
}

void Timer1_ISR(void) interrupt 3
{
    TH1 = (unsigned char)(t1_reload >> 8);
    TL1 = (unsigned char)(t1_reload);
    // 用户代码
}
```

### Timer 0 模式 2（8 位自动重装）

TH0 存放重装值，TL0 溢出后自动从 TH0 加载。ISR 中无需手动重装。

```c
void Timer0_Mode2_Init(unsigned int us)
{
    unsigned long count = (unsigned long)us * (FOSC / 1000000UL) / 12;
    unsigned char reload = (unsigned char)(256UL - count);

    TMOD = 0x02;             // T0 模式 2 (8 位自动重装)
    TH0 = reload;
    TL0 = reload;
    ET0 = 1;
    EA  = 1;
    TR0 = 1;
}

void Timer0_Mode2_ISR(void) interrupt 1
{
    // TL0 已自动从 TH0 重装，无需手动操作
    // 用户代码
}
```

### Timer 1 模式 2（UART 波特率发生器）

```c
#ifndef FOSC
#define FOSC 11059200UL   // 推荐 11.0592 MHz，标准波特率 0% 误差
#endif

void Timer1_BaudRate_Init(unsigned long baud)
{
    unsigned long tmp;

    TMOD = 0x20;             // T1 模式 2 (8 位自动重装，波特率发生器)

    PCON &= ~0x80;         // SMOD = 0
    tmp  = FOSC / 12UL / 32UL / baud;
    TH1  = (unsigned char)(256UL - tmp);
    TL1  = TH1;

    TR1  = 1;
}

// 11.0592 MHz 常用值:
// 9600 -> 0xFD    4800 -> 0xFA    2400 -> 0xF4    1200 -> 0xE8
```

### UART 模式 1（8 位，可变波特率）

```c
void UART_Init(unsigned long baud)
{
    SCON = 0x50;           // 模式 1, REN=1
    Timer1_BaudRate_Init(baud);
    TI = 1;
}

void UART_SendChar(unsigned char c)
{
    TI = 0;
    SBUF = c;
    while (TI == 0);
}

unsigned char UART_RecvChar(void)
{
    while (RI == 0);
    RI = 0;
    return SBUF;
}

void UART_SendString(unsigned char *str)
{
    while (*str) UART_SendChar(*str++);
}

void UART_ISR(void) interrupt 4
{
    if (RI) { RI = 0; /* 处理接收字节 SBUF */ }
    if (TI) { TI = 0; /* 发送完成 */ }
}
```

### PWM 软件方式（Timer 0 查询）

```c
#define PWM_RES  100
sbit PWM_OUT = P1^0;      // 按实际引脚修改

void PWM_Set(unsigned char duty)   // duty: 0 .. PWM_RES
{
    unsigned char i;
    for (i = 0; i < PWM_RES; i++) {
        PWM_OUT = (i < duty);
        Timer0_Mode1_Init(100);    // 每步微秒数，根据所需周期调整
        Timer0_Wait();
    }
}
```

### PWM 中断方式（Timer 0 模式 2 自动重装）

```c
#define PWM_RES  100
sbit PWM_OUT = P1^0;

unsigned char pwm_duty    = 50;
unsigned char pwm_counter = 0;

void PWM_Init(unsigned int period_us)
{
    unsigned int step_us = period_us / PWM_RES;
    Timer0_Mode2_Init(step_us);
    pwm_duty = 50;
}

void PWM_ISR(void) interrupt 1     // Timer 0 模式 2 自动重装
{
    pwm_counter++;
    if (pwm_counter >= PWM_RES) pwm_counter = 0;
    PWM_OUT = (pwm_counter < pwm_duty);
}

void PWM_SetDuty(unsigned char duty)
{
    if (duty > PWM_RES) duty = PWM_RES;
    pwm_duty = duty;
}
```

### 精确阻塞延时（Timer 0）

```c
void Delay_us(unsigned int us)
{
    Timer0_Mode1_Init(us);
    while (TF0 == 0);
    TF0 = 0;
    TR0 = 0;
}

void Delay_ms(unsigned int ms)
{
    while (ms--) Delay_us(1000);
}
```

---

## 项目结构

```
8051LAB/
    finalWork/lib/          公共库 (sysinit, OLED, 传感器, UART 等)
    finalWork/<module>/     各实验模块 (ADC, DAC, MPU, 蓝牙, WiFi 等)
    charts/                 生成的寄存器参考图表
    gen_8051_charts.py      单寄存器/区域图表生成
    gen_8051_overview.py    SFR 总览 + 内存全景生成
```
