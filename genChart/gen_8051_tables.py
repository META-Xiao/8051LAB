#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_8051_tables.py — 标准 8051 MCU 内存单元布局表生成器

以"每个单元格存什么"的视角，生成标准 MCS-51 的 ROM / RAM / SFR 布局。
运行: python gen_8051_tables.py
输出: gen_8051_tables.md
"""

OUTPUT_FILE = "gen_8051_tables.md"

# ═══════════════════════════════════════════════════════════════
#  8048 / 8051 中断向量  —  ROM 最低 128 字节布局
# ═══════════════════════════════════════════════════════════════

def gen_rom_ivt() -> str:
    """生成 ROM 中断向量表——每个地址单元存什么"""
    lines = []
    lines.append("### ROM 低区：中断向量表 & 入口")
    lines.append("")
    lines.append("标准 8051 复位后 PC=0x0000，依次为各中断服务入口，每个间隔仅 8 字节。")
    lines.append("")
    lines.append("| ROM 地址 | 存储内容 | 说明 |")
    lines.append("|:---|:---|:---|")
    lines.append("| `0x0000` | `LJMP MAIN` (3 B) | 复位向量 — 上电后第一条指令，跳转到主程序 |")
    lines.append("| `0x0003` | `LJMP ISR_INT0` (3 B) | **INT0** 外部中断 0 入口 (P3.2) |")
    lines.append("| `0x000B` | `LJMP ISR_T0` (3 B) | **Timer 0** 溢出中断入口 |")
    lines.append("| `0x0013` | `LJMP ISR_INT1` (3 B) | **INT1** 外部中断 1 入口 (P3.3) |")
    lines.append("| `0x001B` | `LJMP ISR_T1` (3 B) | **Timer 1** 溢出中断入口 |")
    lines.append("| `0x0023` | `LJMP ISR_UART` (3 B) | **UART** 串行口中断入口 (TI / RI) |")
    lines.append("| `0x002B` | `LJMP ISR_T2` (3 B) | **Timer 2** 溢出中断入口 (仅 8052) |")
    lines.append("| `0x0033` | 用户代码区开始 | 中断向量表结束，之后为自由程序区 |")
    lines.append("")
    lines.append("> **中断间隔 8 字节**: 若 ISR 极短 (≤5 字节)，可直接写在向量处; 否则放 `LJMP` 跳转。")
    lines.append("")
    return "\n".join(lines)


def gen_rom_layout() -> str:
    """ROM 整体布局"""
    lines = []
    lines.append("### ROM 完整布局 (标准 8051: 4 KB)")
    lines.append("")
    lines.append("| 地址范围 | 大小 | 存储内容 |")
    lines.append("|:---|:---:|:---|")
    lines.append("| `0x0000` – `0x0002` | 3 B | 复位向量：`LJMP main` |")
    lines.append("| `0x0003` – `0x000A` | 8 B | INT0 中断服务入口 |")
    lines.append("| `0x000B` – `0x0012` | 8 B | Timer 0 中断服务入口 |")
    lines.append("| `0x0013` – `0x001A` | 8 B | INT1 中断服务入口 |")
    lines.append("| `0x001B` – `0x0022` | 8 B | Timer 1 中断服务入口 |")
    lines.append("| `0x0023` – `0x002A` | 8 B | UART 中断服务入口 |")
    lines.append("| `0x002B` – `0x0032` | 8 B | Timer 2 中断服务入口 (8052 only) |")
    lines.append("| `0x0033` – `0x0FFF` | ~4 KB | **用户程序区** — C 编译后的代码、常量表 (`code`)、字库 |")
    lines.append("| `0x0000` – `0xFFFF` | 64 KB | **外部 ROM** (EA=0 时使能)，或内部 ROM 超过 4 KB 后自动溢出到外部 |")
    lines.append("")
    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  内部 RAM 低 128 字节  —  逐区画格
# ═══════════════════════════════════════════════════════════════

def gen_ram_low128() -> str:
    """内部 RAM 00H–7FH 各区布局"""
    lines = []
    lines.append("## 一、内部 RAM 低 128 字节 (`0x00` – `0x7F`)")
    lines.append("")
    lines.append("直接 / 间接寻址均可访问。8051 上电后 SP=0x07，堆栈从 0x08 开始生长。")
    lines.append("")

    # ── 工作寄存器区 00H–1FH ──
    lines.append("### 1.1 工作寄存器区 `0x00` – `0x1F` (4 Bank × 8 寄存器)")
    lines.append("")
    lines.append("由 PSW 的 RS1, RS0 选择当前 Bank：")
    lines.append("")
    lines.append("| RS1 | RS0 | 当前 Bank | 地址范围 | R0–R7 映射 |")
    lines.append("|:---:|:---:|:---|:---|:---|")
    lines.append("| 0 | 0 | Bank 0 | `0x00`–`0x07` | R0=00H, R1=01H, R2=02H, R3=03H, R4=04H, R5=05H, R6=06H, R7=07H |")
    lines.append("| 0 | 1 | Bank 1 | `0x08`–`0x0F` | R0=08H, R1=09H, R2=0AH, R3=0BH, R4=0CH, R5=0DH, R6=0EH, R7=0FH |")
    lines.append("| 1 | 0 | Bank 2 | `0x10`–`0x17` | R0=10H, R1=11H, R2=12H, R3=13H, R4=14H, R5=15H, R6=16H, R7=17H |")
    lines.append("| 1 | 1 | Bank 3 | `0x18`–`0x1F` | R0=18H, R1=19H, R2=1AH, R3=1BH, R4=1CH, R5=1DH, R6=1EH, R7=1FH |")
    lines.append("")
    lines.append("> 复位后 RS1:RS0 = 00 → 默认 Bank 0。SP 初始值 0x07 → 堆栈从 0x08 (Bank 1 的 R0) 开始，")
    lines.append("> 因此通常需在 main() 开头将 SP 移到安全位置：`SP = 0x60;`。")
    lines.append("")

    # ── 位寻址区 20H–2FH ──
    lines.append("### 1.2 位寻址区 `0x20` – `0x2F` (16 字节 → 128 位)")
    lines.append("")
    lines.append("每个字节的 8 位都有独立的**位地址**，可直接用 `SETB / CLR / JB / JNB` 操作。")
    lines.append("")
    lines.append("| 字节地址 | bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")

    for byte_addr in range(0x20, 0x30):
        bit_base = (byte_addr - 0x20) * 8
        bits = [f"`{bit_base + i:02X}H`" for i in range(7, -1, -1)]
        lines.append(f"| `{byte_addr:02X}H` | " + " | ".join(bits) + " |")

    lines.append("")
    lines.append("> **位地址公式**: 字节地址 `0x20+N` 的第 `b` 位 → 位地址 = `N×8 + b`  (范围 `0x00`–`0x7F`)")
    lines.append("> 典型用法：`uchar bdata flags; sbit LED = flags^0;` → 编译器自动分配位地址。")
    lines.append("")

    # ── 通用数据区 & 堆栈 30H–7FH ──
    lines.append("### 1.3 通用数据区 / 堆栈区 `0x30` – `0x7F` (80 字节)")
    lines.append("")
    lines.append("| 地址范围 | 大小 | 存储内容 |")
    lines.append("|:---|:---:|:---|")
    lines.append("| `0x30` – `0x5F` | 48 B | **用户变量区** — `data` 关键字变量、局部变量、函数参数 |")
    lines.append("| `0x60` – `0x7F` | 32 B | **推荐堆栈区** — 将 SP 移至此可避开 Bank 1 和位寻址区 |")
    lines.append("| `0x07` (复位 SP) | — | **默认堆栈底** — 第一个 PUSH 写入 0x08，覆盖 Bank 1 的 R0！ |")
    lines.append("")
    lines.append("```c")
    lines.append("// 典型初始化：将 SP 移到安全位置")
    lines.append("SP = 0x60;  // 堆栈从 0x61 开始，远离寄存器区和位寻址区")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  内部 RAM 高 128 字节  —  SFR 区 80H–FFH
# ═══════════════════════════════════════════════════════════════

def gen_sfr_map() -> str:
    """SFR 映射表 — 每个地址存什么寄存器"""
    lines = []
    lines.append("## 二、SFR 区 `0x80` – `0xFF` (特殊功能寄存器)")
    lines.append("")
    lines.append("高 128 字节地址空间中，**直接寻址**访问 SFR，**间接寻址** (`@R0/@R1`) 访问普通 RAM (8052)。")
    lines.append("只有地址能被 8 整除的 SFR 才可**位寻址** (即地址末位为 0 或 8)。")
    lines.append("")

    # 按地址列出 SFR
    sfrs = [
        (0x80, "P0", "✓", "端口 0 — 开漏 I/O，需外部上拉"),
        (0x81, "SP", "✗", "堆栈指针 — 复位=0x07，PUSH 前先+1"),
        (0x82, "DPL", "✗", "数据指针低字节"),
        (0x83, "DPH", "✗", "数据指针高字节"),
        (0x87, "PCON", "✗", "电源控制 — SMOD(bit7) 波特率倍速; GF1/GF0; PD/IDL 休眠"),
        (0x88, "TCON", "✓", "定时器控制 (见 §三)"),
        (0x89, "TMOD", "✗", "定时器模式 (见 §四)"),
        (0x8A, "TL0", "✗", "Timer 0 低 8 位"),
        (0x8B, "TL1", "✗", "Timer 1 低 8 位"),
        (0x8C, "TH0", "✗", "Timer 0 高 8 位"),
        (0x8D, "TH1", "✗", "Timer 1 高 8 位"),
        (0x90, "P1", "✓", "端口 1 — 内部上拉 I/O"),
        (0x98, "SCON", "✓", "串行口控制 (见 §七)"),
        (0x99, "SBUF", "✗", "串行口数据缓冲 — 读写同一地址，读=接收, 写=发送"),
        (0xA0, "P2", "✓", "端口 2 — 内部上拉 I/O，也是外部存储器高 8 位地址"),
        (0xA8, "IE", "✓", "中断使能 (见 §五)"),
        (0xB0, "P3", "✓", "端口 3 — 内部上拉 I/O，复用第二功能 (RXD/TXD/INT0/INT1/T0/T1/WR/RD)"),
        (0xB8, "IP", "✓", "中断优先级 (见 §六)"),
        (0xD0, "PSW", "✓", "程序状态字 — CY/AC/F0/RS1/RS0/OV/—/P"),
        (0xE0, "ACC", "✓", "累加器 A — 所有算术/逻辑运算的核心"),
        (0xF0, "B", "✓", "B 寄存器 — MUL/DIV 的第二个操作数"),
    ]

    # 8052 only
    sfrs_52 = [
        (0xC8, "T2CON", "✓", "Timer 2 控制 (8052)"),
        (0xC9, "T2MOD", "✗", "Timer 2 模式 (8052)"),
        (0xCA, "RCAP2L", "✗", "Timer 2 捕获/重装低字节 (8052)"),
        (0xCB, "RCAP2H", "✗", "Timer 2 捕获/重装高字节 (8052)"),
        (0xCC, "TL2", "✗", "Timer 2 低 8 位 (8052)"),
        (0xCD, "TH2", "✗", "Timer 2 高 8 位 (8052)"),
    ]

    lines.append("| 地址 | 寄存器 | 位寻址 | 存储内容 |")
    lines.append("|:---|:---|:---:|:---|")

    # Merge and sort all SFRs (excluding duplicates between standard and 8052)
    all_sfrs = {}  # addr -> (name, bitaddr, desc)
    for addr, name, bitaddr, desc in sfrs:
        all_sfrs[addr] = (name, bitaddr, desc)
    for addr, name, bitaddr, desc in sfrs_52:
        if addr not in all_sfrs:
            all_sfrs[addr] = (name, bitaddr, desc)

    # 打印 80-FF 每个地址
    for addr in range(0x80, 0x100):
        if addr in all_sfrs:
            name, bitaddr, desc = all_sfrs[addr]
            lines.append(f"| `{addr:02X}H` | **{name}** | {bitaddr} | {desc} |")
        else:
            # Check if reserved / unused
            low_nib = addr % 16
            if low_nib in (0, 8):
                lines.append(f"| `{addr:02X}H` | *(保留)* | — | 可位寻址但未定义寄存器 |")
            else:
                lines.append(f"| `{addr:02X}H` | *(保留)* | — | — |")

    lines.append("")
    lines.append("> 未列出的地址为保留单元，不可使用。部分地址在增强型 8051 中映射到扩展外设。")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  3. TCON  —  定时器控制寄存器
# ═══════════════════════════════════════════════════════════════

def gen_tcon() -> str:
    lines = []
    lines.append("## 三、TCON — 定时器/计数器控制寄存器 (`0x88`, 可位寻址)")
    lines.append("")
    lines.append("TCON 同时控制 Timer 0/1 的启停 + 外部中断触发方式。")
    lines.append("")
    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | **TF1** | **TR1** | **TF0** | **TR0** | **IE1** | **IT1** | **IE0** | **IT0** |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("| **所属** | Timer 1 | Timer 1 | Timer 0 | Timer 0 | INT1 | INT1 | INT0 | INT0 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7 | **TF1** | **Timer 1 溢出标志** — FFFF→0000 跳变时硬件置 1；CPU 响应中断后硬件清 0；若用查询方式需软件 `TF1=0;` |")
    lines.append("| 6 | **TR1** | **Timer 1 运行控制** — `TR1=1` 启动 T1；`TR1=0` 停止 T1 (GATE=0 时) |")
    lines.append("| 5 | **TF0** | **Timer 0 溢出标志** — FFFF→0000 跳变时硬件置 1；中断响应后硬件清 0 |")
    lines.append("| 4 | **TR0** | **Timer 0 运行控制** — `TR0=1` 启动 T0；`TR0=0` 停止 T0 (GATE=0 时) |")
    lines.append("| 3 | **IE1** | **INT1 边沿标志** — 检测到 INT1 (P3.3) 有效边沿时硬件置 1；中断响应后硬件清 0 |")
    lines.append("| 2 | **IT1** | **INT1 触发类型** — `0`: 低电平触发 (电平敏感); `1`: 下降沿触发 (边沿敏感) |")
    lines.append("| 1 | **IE0** | **INT0 边沿标志** — 检测到 INT0 (P3.2) 有效边沿时硬件置 1；中断响应后硬件清 0 |")
    lines.append("| 0 | **IT0** | **INT0 触发类型** — `0`: 低电平触发；`1`: 下降沿触发 |")
    lines.append("")

    lines.append("```c")
    lines.append("// 常用 TCON 配置")
    lines.append("TCON = 0x50;  // TR1=1, TR0=1   — 同时启动 T0 & T1")
    lines.append("TCON = 0x40;  // TR1=1           — 仅启动 T1 (UART 波特率源)")
    lines.append("TCON = 0x10;  // TR0=1           — 仅启动 T0 (通用计时)")
    lines.append("TCON = 0x01;  // IT0=1           — INT0 下降沿触发")
    lines.append("TCON = 0x05;  // IT0=1, IT1=1   — 两个外部中断均边沿触发")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  4. TMOD  —  定时器模式寄存器
# ═══════════════════════════════════════════════════════════════

def gen_tmod() -> str:
    lines = []
    lines.append("## 四、TMOD — 定时器/计数器模式寄存器 (`0x89`, 不可位寻址)")
    lines.append("")
    lines.append("TMOD 设置 Timer 0 和 Timer 1 的工作模式。高 4 位控制 T1，低 4 位控制 T0。**不可位寻址**，必须整字节写入。")
    lines.append("")

    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | **GATE** | **C/T̅** | **M1** | **M0** | **GATE** | **C/T̅** | **M1** | **M0** |")
    lines.append("| **所属** | T1 | T1 | T1 | T1 | T0 | T0 | T0 | T0 |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7,3 | **GATE** | **门控位** — `0`: TRx=1 即运行; `1`: TRx=1 **且** INTx 引脚=1 时才运行 (用于测量外部脉冲宽度) |")
    lines.append("| 6,2 | **C/T̅** | **计数/定时选择** — `0`: 定时器 (对机器周期计数, f<sub>OSC</sub>/12); `1`: 计数器 (对 Tx 引脚负跳变计数) |")
    lines.append("| 5,4 / 1,0 | **M1,M0** | **工作模式选择** — 见下表 |")
    lines.append("")

    lines.append("### Timer 工作模式")
    lines.append("")
    lines.append("| M1 | M0 | 模式 | 位数 | 行为 | 典型用途 |")
    lines.append("|:---:|:---:|:---|:---|:---|:---|")
    lines.append("| 0 | 0 | **模式 0** | 13 位 | TH(8 bit) + TL(低5 bit)，兼容 8048 | 极少使用 |")
    lines.append("| 0 | 1 | **模式 1** | 16 位 | TH(8 bit) + TL(8 bit)，手动重装 | 精确计时 / 事件计数 |")
    lines.append("| 1 | 0 | **模式 2** | 8 位自动重装 | TL 计数溢出 → TH 自动重装到 TL，TH 不变 | **UART 波特率发生器** (T1) |")
    lines.append("| 1 | 1 | **模式 3** | 双 8 位 (仅 T0) | T0 拆为 TL0(8位)+TH0(8位) 两个独立定时器；T1 此时停止 | 需要三个定时器时 |")
    lines.append("")

    lines.append("```c")
    lines.append("// 常用 TMOD 配置")
    lines.append("TMOD = 0x01;  // T0: 模式1 (16位), T1: 模式0     — 单定时器精确计时")
    lines.append("TMOD = 0x11;  // T0+T1: 均模式1 (16位)         — 双 16 位定时器")
    lines.append("TMOD = 0x20;  // T1: 模式2 (8位自动重装)        — UART 波特率  (经典配置)")
    lines.append("TMOD = 0x22;  // T0+T1: 均模式2 (自动重装)      — 双波特率发生器")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  5. IE  —  中断使能寄存器
# ═══════════════════════════════════════════════════════════════

def gen_ie() -> str:
    lines = []
    lines.append("## 五、IE — 中断使能寄存器 (`0xA8`, 可位寻址)")
    lines.append("")
    lines.append("每个位控制一个中断源的开关。EA 是总闸。")
    lines.append("")

    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | **EA** | — | **ET2** | **ES** | **ET1** | **EX1** | **ET0** | **EX0** |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("| **控制** | 总中断 | (保留) | T2 | UART | T1 | INT1 | T0 | INT0 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7 | **EA** | **Enable All — 总中断开关** — `1`: 各中断由自己的使能位控制; `0`: **禁止所有中断** (无论各使能位状态) |")
    lines.append("| 6 | — | **保留** — 未定义，读/写无意义 |")
    lines.append("| 5 | **ET2** | **Enable Timer 2** — `1`: 允许 T2 溢出/捕获中断 (仅 8052/AT89C52) |")
    lines.append("| 4 | **ES** | **Enable Serial** — `1`: 允许串行口中断 (TI=1 或 RI=1 时触发) |")
    lines.append("| 3 | **ET1** | **Enable Timer 1** — `1`: 允许 T1 溢出中断 (TF1=1 时触发) |")
    lines.append("| 2 | **EX1** | **Enable External 1** — `1`: 允许 INT1 (P3.3) 外部中断 |")
    lines.append("| 1 | **ET0** | **Enable Timer 0** — `1`: 允许 T0 溢出中断 (TF0=1 时触发) |")
    lines.append("| 0 | **EX0** | **Enable External 0** — `1`: 允许 INT0 (P3.2) 外部中断 |")
    lines.append("")

    lines.append("```c")
    lines.append("// 常用 IE 配置")
    lines.append("IE = 0x00;  // 禁止所有中断")
    lines.append("IE = 0x80;  // EA=1 仅开总中断 (外设自行使能)")
    lines.append("IE = 0x82;  // EA + ET0     — Timer 0 中断")
    lines.append("IE = 0x88;  // EA + ES      — 串口中断")
    lines.append("IE = 0x90;  // EA + ET1 + ES — T1 (波特率) + 串口")
    lines.append("IE = 0x81;  // EA + EX0     — 外部中断 0")
    lines.append("IE = 0x85;  // EA + EX0 + ET0 — INT0 + Timer0")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  6. IP  —  中断优先级寄存器
# ═══════════════════════════════════════════════════════════════

def gen_ip() -> str:
    lines = []
    lines.append("## 六、IP — 中断优先级寄存器 (`0xB8`, 可位寻址)")
    lines.append("")
    lines.append("8051 的中断只有**两级优先级**：0 (低) 和 1 (高)。高优先级 ISR 可打断低优先级 ISR。")
    lines.append("同级中断同时触发时，按**自然优先级**顺序响应。")
    lines.append("")

    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | — | — | **PT2** | **PS** | **PT1** | **PX1** | **PT0** | **PX0** |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("| **控制** | (保留) | (保留) | T2 优先级 | UART 优先级 | T1 优先级 | INT1 优先级 | T0 优先级 | INT0 优先级 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7,6 | — | **保留** — 未定义 |")
    lines.append("| 5 | **PT2** | **Timer 2 优先级** — `1`: 高优先级; `0`: 低优先级 (8052) |")
    lines.append("| 4 | **PS** | **串行口优先级** — `1`: 高优先级; `0`: 低优先级 |")
    lines.append("| 3 | **PT1** | **Timer 1 优先级** — `1`: 高优先级; `0`: 低优先级 |")
    lines.append("| 2 | **PX1** | **INT1 优先级** — `1`: 高优先级; `0`: 低优先级 |")
    lines.append("| 1 | **PT0** | **Timer 0 优先级** — `1`: 高优先级; `0`: 低优先级 |")
    lines.append("| 0 | **PX0** | **INT0 优先级** — `1`: 高优先级; `0`: 低优先级 |")
    lines.append("")

    lines.append("### 自然优先级 (同级同时触发时的响应顺序)")
    lines.append("")
    lines.append("| 优先级 | 中断源 | 向量地址 | 查询顺序 |")
    lines.append("|:---:|:---|---:|:---|")
    lines.append("| 1 (最高) | **INT0** — 外部中断 0 | `0x0003` | 硬件在每个机器周期按此顺序轮询 |")
    lines.append("| 2 | **Timer 0** — 定时器 0 溢出 | `0x000B` | |")
    lines.append("| 3 | **INT1** — 外部中断 1 | `0x0013` | |")
    lines.append("| 4 | **Timer 1** — 定时器 1 溢出 | `0x001B` | |")
    lines.append("| 5 | **UART** — 串行口收发 | `0x0023` | |")
    lines.append("| 6 (最低) | **Timer 2** — 定时器 2 (8052) | `0x002B` | |")
    lines.append("")

    lines.append("> 注意：自然优先级仅决定「同时触发」时的响应顺序。一旦某个低优先级 ISR 正在执行，")
    lines.append("> 即便更高自然优先级的中断触发，也必须等待当前 ISR 完成 (除非新中断设为高优先级)。")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  7. SCON
# ═══════════════════════════════════════════════════════════════

def gen_scon() -> str:
    lines = []
    lines.append("## 七、SCON — 串行口控制寄存器 (`0x98`, 可位寻址)")
    lines.append("")

    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | **SM0/FE** | **SM1** | **SM2** | **REN** | **TB8** | **RB8** | **TI** | **RI** |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7,6 | **SM0, SM1** | **串口模式选择** — 见下表 |")
    lines.append("| 5 | **SM2** | **多机通信使能** — 模式 2/3: `1` 时若 RB8=0 则 RI 不置位 (用于地址识别) |")
    lines.append("| 4 | **REN** | **接收使能** — `1`: 允许接收; `0`: 禁止接收 |")
    lines.append("| 3 | **TB8** | **发送位 9** — 模式 2/3 时作为第 9 数据位发送 (多机通信的地址/数据标志) |")
    lines.append("| 2 | **RB8** | **接收位 9** — 模式 2/3 时存储接收到的第 9 位 |")
    lines.append("| 1 | **TI** | **发送中断标志** — 发送完一帧后硬件置 1；**必须软件清 0** (`TI=0;`) |")
    lines.append("| 0 | **RI** | **接收中断标志** — 接收完一帧后硬件置 1；**必须软件清 0** (`RI=0;`) |")
    lines.append("")

    lines.append("### UART 工作模式")
    lines.append("")
    lines.append("| SM0 | SM1 | 模式 | 帧格式 | 波特率 |")
    lines.append("|:---:|:---:|:---|:---|:---|")
    lines.append("| 0 | 0 | **模式 0** | 8 位同步移位 | f<sub>OSC</sub>/12 (固定) |")
    lines.append("| 0 | 1 | **模式 1** | 10 位 (1 start + 8 data + 1 stop) | **可变** = (2<sup>SMOD</sup>/32) × T1 溢出率 |")
    lines.append("| 1 | 0 | **模式 2** | 11 位 (+ 第9位) | f<sub>OSC</sub>/64 或 f<sub>OSC</sub>/32 (SMOD 控制) |")
    lines.append("| 1 | 1 | **模式 3** | 11 位 (+ 第9位) | **可变** = (2<sup>SMOD</sup>/32) × T1 溢出率 |")
    lines.append("")

    lines.append("> **最常用**: `SCON = 0x50` (模式 1 + REN=1) → 8 位 UART，T1 产生波特率。")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  8. PSW  —  程序状态字
# ═══════════════════════════════════════════════════════════════

def gen_psw() -> str:
    lines = []
    lines.append("## 八、PSW — 程序状态字 (`0xD0`, 可位寻址)")
    lines.append("")

    lines.append("| 位 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |")
    lines.append("|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|")
    lines.append("| **名称** | **CY** | **AC** | **F0** | **RS1** | **RS0** | **OV** | — | **P** |")
    lines.append("| **复位值** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |")
    lines.append("")

    lines.append("| 位 | 名称 | 存储内容 / 行为 |")
    lines.append("|:---|:---|:---|")
    lines.append("| 7 | **CY** | **进位标志** — 加法有进位/减法有借位置 1；也是布尔累加器 (`SETB C; MOV C, bit`) |")
    lines.append("| 6 | **AC** | **辅助进位** — 低 4 位向高 4 位有进位/借位置 1 (用于 BCD 调整 `DA A`) |")
    lines.append("| 5 | **F0** | **用户标志 0** — 用户自由使用的通用标志位 |")
    lines.append("| 4 | **RS1** | **寄存器 Bank 选择高** — 与 RS0 组合选择 R0–R7 映射的 Bank (0–3) |")
    lines.append("| 3 | **RS0** | **寄存器 Bank 选择低** — RS1:RS0=00→Bank0, 01→Bank1, 10→Bank2, 11→Bank3 |")
    lines.append("| 2 | **OV** | **溢出标志** — 有符号运算结果超出 -128~+127 范围时置 1 |")
    lines.append("| 1 | — | **保留** |")
    lines.append("| 0 | **P** | **奇偶标志** — ACC 中 1 的个数为奇数时硬件置 1 (每个指令周期自动更新) |")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  9. 波特率计算参考
# ═══════════════════════════════════════════════════════════════

def gen_baudrate() -> str:
    lines = []
    lines.append("## 九、标准 8051 波特率计算 (T1 模式 2)")
    lines.append("")
    lines.append("UART 模式 1/3 的波特率由 Timer 1 溢出率决定：")
    lines.append("")
    lines.append("> **波特率 = (2<sup>SMOD</sup> / 32) × (f<sub>OSC</sub> / 12) / (256 − TH1)**")
    lines.append("")
    lines.append("### 常用晶振下的 TH1 重装值")
    lines.append("")
    lines.append("| 波特率 | f<sub>OSC</sub> = 11.0592 MHz | f<sub>OSC</sub> = 12 MHz | SMOD |")
    lines.append("|:---|:---:|:---:|:---:|")
    lines.append("| 9600 | `TH1 = 0xFD` (误差 0%) | `TH1 = 0xF9` (误差 6.99%) | 0 |")
    lines.append("| 4800 | `TH1 = 0xFA` (误差 0%) | `TH1 = 0xF3` (误差 0.16%) | 0 |")
    lines.append("| 2400 | `TH1 = 0xF4` (误差 0%) | `TH1 = 0xE6` (误差 0.16%) | 0 |")
    lines.append("| 1200 | `TH1 = 0xE8` (误差 0%) | `TH1 = 0xCC` (误差 0.16%) | 0 |")
    lines.append("| 19200 | `TH1 = 0xFD` + SMOD=1 (误差 0%) | — | 1 |")
    lines.append("")
    lines.append("> **11.0592 MHz** 是 UART 通信的首选晶振，所有标准波特率均 0% 误差。")
    lines.append("> 本项目 C8051F410 使用内部 24.5 MHz 振荡器 + 硬件波特率发生器，不使用 T1。")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  10. 外部 RAM / ROM 引脚复用
# ═══════════════════════════════════════════════════════════════

def gen_ext_mem() -> str:
    lines = []
    lines.append("## 十、外部存储器访问 (总线复用)")
    lines.append("")
    lines.append("当片内 ROM/RAM 不够时，8051 通过 P0/P2 访问外部存储器，地址/数据分时复用：")
    lines.append("")
    lines.append("| 信号 | 引脚 | 功能 |")
    lines.append("|:---|:---|:---|")
    lines.append("| **ALE** | P3 复用 / 专用引脚 | **地址锁存使能** — 下降沿时将 P0 上的低 8 位地址锁存到 74HC373 |")
    lines.append("| **PSEN̅** | 专用引脚 | **程序存储使能** — 低电平时从外部 ROM 读指令 (MOVC) |")
    lines.append("| **WR̅** | P3.6 | **外部 RAM 写** — 低电平时将数据写入外部 RAM (MOVX @DPTR, A) |")
    lines.append("| **RD̅** | P3.7 | **外部 RAM 读** — 低电平时从外部 RAM 读数据 (MOVX A, @DPTR) |")
    lines.append("| **P0** | P0.0–P0.7 | 低 8 位地址 + 8 位数据分时复用 (需锁存器分离) |")
    lines.append("| **P2** | P2.0–P2.7 | 高 8 位地址 (64 KB 范围寻址) |")
    lines.append("| **EA̅** | 专用引脚 | `EA=1`: 使用内部 ROM; `EA=0`: 全部使用外部 ROM |")
    lines.append("")

    lines.append("```")
    lines.append("典型外部存储器连接:")
    lines.append("")
    lines.append("  8051        74HC373      外部 ROM/RAM")
    lines.append("  ─────      ────────      ──────────")
    lines.append("  P0[0:7] ──┬── D[0:7]      数据总线 D[0:7]")
    lines.append("            │")
    lines.append("            └── Q[0:7] ──── 地址 A[0:7] (低 8 位)")
    lines.append("  P2[0:7] ──────────────── 地址 A[8:15] (高 8 位)")
    lines.append("  ALE     ──── LE ──────── (锁存使能)")
    lines.append("  PSEN    ──────────────── ROM OE̅")
    lines.append("  RD/WR   ──────────────── RAM OE̅/WE̅")
    lines.append("```")
    lines.append("")

    return "\n".join(lines)


# ═══════════════════════════════════════════════════════════════
#  MAIN
# ═══════════════════════════════════════════════════════════════

def main():
    parts = [
        ("# 标准 MCS-51 (8051) 存储器 & 寄存器单元布局", ""),
        (gen_ram_low128(), ""),
        (gen_sfr_map(), ""),
        (gen_tcon(), ""),
        (gen_tmod(), ""),
        (gen_ie(), ""),
        (gen_ip(), ""),
        (gen_scon(), ""),
        (gen_psw(), ""),
        (gen_rom_ivt(), ""),
        (gen_rom_layout(), ""),
        (gen_baudrate(), ""),
        (gen_ext_mem(), ""),
    ]

    content = "\n\n".join(p[0] for p in parts if p[0])
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"Generated: {OUTPUT_FILE}  ({len(content)} chars, {content.count(chr(10))} lines)")


if __name__ == "__main__":
    main()
