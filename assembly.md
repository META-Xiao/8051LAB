# 8051 汇编指令复习手册

## 目录
1. [寻址方式](#1-寻址方式)
2. [数据传送指令](#2-数据传送指令)
3. [算术运算指令](#3-算术运算指令)
4. [逻辑运算指令](#4-逻辑运算指令)
5. [控制转移指令](#5-控制转移指令)
6. [位操作指令](#6-位操作指令)
7. [伪指令](#7-伪指令)
8. [代码模板](#8-代码模板)
9. [考试重点](#9-考试重点)


## 1. 寻址方式

8051 共有 **5 种寻址方式**：

| 寻址方式 | 示例 | 说明 | 字节数 |
|:---|:---|:---|:---:|
| **立即寻址** | `MOV A, #40H` | 操作数直接跟在指令后 | 2 |
| **直接寻址** | `MOV A, 40H` | 直接给出 RAM/SFR 地址 | 2 |
| **寄存器寻址** | `MOV A, R0` | 操作数在工作寄存器中 | 1 |
| **寄存器间接寻址** | `MOV A, @R0` | R0/R1/DPTR 存放地址 | 1 |
| **位寻址** | `MOV C, 20H.3` | 直接访问某一位 | 2 |

### 考试陷阱

```assembly
MOV A, 40H    ; A = 内部RAM 40H单元的内容 (直接寻址)
MOV A, #40H   ; A = 40H 这个立即数 (立即寻址)
; 两条指令编译后都是 2 字节，但含义完全不同！
```


## 2. 数据传送指令

### 2.1 内部 RAM 传送

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `MOV A, Rn` | 寄存器→A | 1 | 1 |
| `MOV A, direct` | 直接地址→A | 2 | 1 |
| `MOV A, @Ri` | 间接地址→A | 1 | 1 |
| `MOV A, #data` | 立即数→A | 2 | 1 |
| `MOV Rn, A` | A→寄存器 | 1 | 1 |
| `MOV Rn, direct` | 直接地址→寄存器 | 2 | 2 |
| `MOV Rn, #data` | 立即数→寄存器 | 2 | 1 |
| `MOV direct, A` | A→直接地址 | 2 | 1 |
| `MOV direct, Rn` | 寄存器→直接地址 | 2 | 2 |
| `MOV direct1, direct2` | 直接地址→直接地址 | 3 | 2 |
| `MOV direct, @Ri` | 间接地址→直接地址 | 2 | 2 |
| `MOV direct, #data` | 立即数→直接地址 | 3 | 2 |
| `MOV @Ri, A` | A→间接地址 | 1 | 1 |
| `MOV @Ri, direct` | 直接地址→间接地址 | 2 | 2 |
| `MOV @Ri, #data` | 立即数→间接地址 | 2 | 1 |

### 2.2 外部 RAM 传送

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `MOVX A, @Ri` | 外部RAM→A (低8位地址) | 1 | 2 |
| `MOVX A, @DPTR` | 外部RAM→A (16位地址) | 1 | 2 |
| `MOVX @Ri, A` | A→外部RAM (低8位地址) | 1 | 2 |
| `MOVX @DPTR, A` | A→外部RAM (16位地址) | 1 | 2 |

### 2.3 查表指令 (MOVC)

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `MOVC A, @A+DPTR` | 程序存储器查表 (DPTR基址) | 1 | 2 |
| `MOVC A, @A+PC` | 程序存储器查表 (PC基址) | 1 | 2 |

### 2.4 交换指令

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `XCH A, Rn` | A与Rn交换 | 1 | 1 |
| `XCH A, direct` | A与直接地址交换 | 2 | 1 |
| `XCH A, @Ri` | A与间接地址交换 | 1 | 1 |
| `XCHD A, @Ri` | A低4位与间接地址低4位交换 | 1 | 1 |
| `SWAP A` | A高低4位互换 | 1 | 1 |

### 2.5 堆栈操作

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `PUSH direct` | SP先加1，再压栈 | 2 | 2 |
| `POP direct` | 先出栈，SP再减1 | 2 | 2 |

**重要**：PUSH/POP 只能操作直接地址，不能是寄存器！
```assembly
PUSH ACC    ; 正确 (ACC是直接地址 0E0H)
PUSH A      ; 错误！
PUSH 0E0H   ; 正确 (等效于 PUSH ACC)
```


## 3. 算术运算指令

### 3.1 加法

| 指令 | 说明 | 影响标志 |
|:---|:---|:---|
| `ADD A, Rn` | A = A + Rn | CY, OV, AC, P |
| `ADD A, direct` | A = A + direct | CY, OV, AC, P |
| `ADD A, @Ri` | A = A + @Ri | CY, OV, AC, P |
| `ADD A, #data` | A = A + data | CY, OV, AC, P |
| `ADDC A, Rn` | A = A + Rn + CY | CY, OV, AC, P |
| `ADDC A, direct` | A = A + direct + CY | CY, OV, AC, P |
| `ADDC A, @Ri` | A = A + @Ri + CY | CY, OV, AC, P |
| `ADDC A, #data` | A = A + data + CY | CY, OV, AC, P |

### 3.2 减法

| 指令 | 说明 | 影响标志 |
|:---|:---|:---|
| `SUBB A, Rn` | A = A - Rn - CY | CY, OV, AC, P |
| `SUBB A, direct` | A = A - direct - CY | CY, OV, AC, P |
| `SUBB A, @Ri` | A = A - @Ri - CY | CY, OV, AC, P |
| `SUBB A, #data` | A = A - data - CY | CY, OV, AC, P |

**注意**：8051 没有不带借位的减法！要减必须先 `CLR C`。

### 3.3 乘除法

| 指令 | 说明 | 结果 |
|:---|:---|:---|
| `MUL AB` | A × B | 结果16位：高8位在B，低8位在A |
| `DIV AB` | A ÷ B | 商在A，余数在B |

### 3.4 加1减1

| 指令 | 说明 | 字节 | 周期 | 影响标志 |
|:---|:---|:---:|:---:|:---|
| `INC A` | A = A + 1 | 1 | 1 | P |
| `INC Rn` | Rn = Rn + 1 | 1 | 1 | 无 |
| `INC direct` | direct = direct + 1 | 2 | 1 | 无 |
| `INC @Ri` | @Ri = @Ri + 1 | 1 | 1 | 无 |
| `INC DPTR` | DPTR = DPTR + 1 | 1 | 2 | 无 |
| `DEC A` | A = A - 1 | 1 | 1 | P |
| `DEC Rn` | Rn = Rn - 1 | 1 | 1 | 无 |
| `DEC direct` | direct = direct - 1 | 2 | 1 | 无 |
| `DEC @Ri` | @Ri = @Ri - 1 | 1 | 1 | 无 |

### 3.5 BCD调整

| 指令 | 说明 |
|:---|:---|
| `DA A` | BCD加法调整 (必须紧跟在加法指令后) |


## 4. 逻辑运算指令

### 4.1 与、或、异或

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `ANL A, Rn` | A = A & Rn | 1 | 1 |
| `ANL A, direct` | A = A & direct | 2 | 1 |
| `ANL A, @Ri` | A = A & @Ri | 1 | 1 |
| `ANL A, #data` | A = A & data | 2 | 1 |
| `ANL direct, A` | direct = direct & A | 2 | 1 |
| `ANL direct, #data` | direct = direct & data | 3 | 2 |
| `ORL A, Rn` | A = A \| Rn | 1 | 1 |
| `ORL A, direct` | A = A \| direct | 2 | 1 |
| `ORL A, @Ri` | A = A \| @Ri | 1 | 1 |
| `ORL A, #data` | A = A \| data | 2 | 1 |
| `ORL direct, A` | direct = direct \| A | 2 | 1 |
| `ORL direct, #data` | direct = direct \| data | 3 | 2 |
| `XRL A, Rn` | A = A ^ Rn | 1 | 1 |
| `XRL A, direct` | A = A ^ direct | 2 | 1 |
| `XRL A, @Ri` | A = A ^ @Ri | 1 | 1 |
| `XRL A, #data` | A = A ^ data | 2 | 1 |
| `XRL direct, A` | direct = direct ^ A | 2 | 1 |
| `XRL direct, #data` | direct = direct ^ data | 3 | 2 |

### 4.2 移位指令

| 指令 | 说明 | 操作 |
|:---|:---|:---|
| `RL A` | 循环左移 | A7→A0, Ai→Ai+1 |
| `RLC A` | 带进位循环左移 | A7→CY, CY→A0, Ai→Ai+1 |
| `RR A` | 循环右移 | A0→A7, Ai→Ai-1 |
| `RRC A` | 带进位循环右移 | A0→CY, CY→A7, Ai→Ai-1 |

### 4.3 清零取反

| 指令 | 说明 |
|:---|:---|
| `CLR A` | A清零 |
| `CPL A` | A取反 |

---

## 5. 控制转移指令

### 5.1 无条件跳转

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `LJMP addr16` | 长跳转 (64KB范围) | 3 | 2 |
| `AJMP addr11` | 绝对跳转 (2KB范围) | 2 | 2 |
| `SJMP rel` | 短跳转 (-128~+127) | 2 | 2 |
| `JMP @A+DPTR` | 间接跳转 (散转) | 1 | 2 |

### 5.2 条件跳转

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `JZ rel` | A为0则跳转 | 2 | 2 |
| `JNZ rel` | A不为0则跳转 | 2 | 2 |
| `JC rel` | CY=1则跳转 | 2 | 2 |
| `JNC rel` | CY=0则跳转 | 2 | 2 |
| `JB bit, rel` | bit=1则跳转 | 3 | 2 |
| `JNB bit, rel` | bit=0则跳转 | 3 | 2 |
| `JBC bit, rel` | bit=1则跳转，并清零该位 | 3 | 2 |

### 5.3 比较跳转

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `CJNE A, direct, rel` | A≠direct则跳转 | 3 | 2 |
| `CJNE A, #data, rel` | A≠data则跳转 | 3 | 2 |
| `CJNE Rn, #data, rel` | Rn≠data则跳转 | 3 | 2 |
| `CJNE @Ri, #data, rel` | @Ri≠data则跳转 | 3 | 2 |

**CJNE 的隐含操作**：不等时会设置 CY
- 目标操作数 < 源操作数 → CY = 1
- 目标操作数 ≥ 源操作数 → CY = 0

### 5.4 循环跳转

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `DJNZ Rn, rel` | Rn=Rn-1，若Rn≠0则跳转 | 2 | 2 |
| `DJNZ direct, rel` | direct=direct-1，若direct≠0则跳转 | 3 | 2 |

### 5.5 子程序调用与返回

| 指令 | 说明 | 字节 | 周期 |
|:---|:---|:---:|:---:|
| `LCALL addr16` | 长调用 (64KB范围) | 3 | 2 |
| `ACALL addr11` | 绝对调用 (2KB范围) | 2 | 2 |
| `RET` | 子程序返回 | 1 | 2 |
| `RETI` | 中断返回 | 1 | 2 |

**RET vs RETI**：
- `RET`：从子程序返回，只恢复 PC
- `RETI`：从中断返回，恢复 PC **并**清除中断优先级触发器

### 5.6 空操作

| 指令 | 说明 |
|:---|:---|
| `NOP` | 空操作，占用1字节1周期 |

## 6. 位操作指令

### 6.1 位传送

| 指令 | 说明 |
|:---|:---|
| `MOV C, bit` | bit→CY |
| `MOV bit, C` | CY→bit |

### 6.2 位逻辑

| 指令 | 说明 |
|:---|:---|
| `ANL C, bit` | CY = CY & bit |
| `ANL C, /bit` | CY = CY & ~bit |
| `ORL C, bit` | CY = CY \| bit |
| `ORL C, /bit` | CY = CY \| ~bit |
| `CPL C` | CY取反 |
| `CPL bit` | bit取反 |

### 6.3 位清零/置位

| 指令 | 说明 |
|:---|:---|
| `CLR C` | CY清零 |
| `CLR bit` | bit清零 |
| `SETB C` | CY置1 |
| `SETB bit` | bit置1 |

## 7. 伪指令

| 伪指令 | 说明 | 示例 |
|:---|:---|:---|
| `ORG` | 设置起始地址 | `ORG 0000H` |
| `END` | 程序结束 | `END` |
| `EQU` | 等值定义 | `COUNT EQU 10` |
| `DB` | 定义字节 | `DB 'Hello', 0` |
| `DW` | 定义字 | `DW 1234H` |
| `DS` | 预留空间 | `DS 10` |
| `BIT` | 位定义 | `LED BIT P1.0` |
| `DATA` | 数据地址定义 | `TEMP DATA 30H` |
| `XDATA` | 外部数据定义 | `BUF XDATA 1000H` |


## 8. 代码模板

### 8.1 程序框架

```assembly
;========================================
; 8051 汇编程序基本框架
;========================================
        ORG 0000H           ; 复位入口
        LJMP MAIN           ; 跳转到主程序

        ORG 0003H           ; 外部中断0入口
        LJMP EX0_ISR

        ORG 000BH           ; 定时器0中断入口
        LJMP T0_ISR

        ORG 001BH           ; 定时器1中断入口
        LJMP T1_ISR

        ORG 0023H           ; 串口中断入口
        LJMP UART_ISR

        ORG 0033H           ; 用户程序起始
;----------------------------------------
; 主程序
;----------------------------------------
MAIN:
        MOV SP, #60H        ; 设置堆栈指针
        ; 初始化代码...
        ; 主循环...
LOOP:
        ; 你的代码
        SJMP LOOP

;----------------------------------------
; 中断服务程序
;----------------------------------------
EX0_ISR:
        PUSH ACC
        PUSH PSW
        ; 中断处理代码
        POP PSW
        POP ACC
        RETI

T0_ISR:
        PUSH ACC
        PUSH PSW
        ; 定时器0处理代码
        POP PSW
        POP ACC
        RETI

T1_ISR:
        PUSH ACC
        PUSH PSW
        ; 定时器1处理代码
        POP PSW
        POP ACC
        RETI

UART_ISR:
        PUSH ACC
        PUSH PSW
        ; 串口处理代码
        POP PSW
        POP ACC
        RETI

        END
```

### 8.2 数据块搬运 (单层循环)

```assembly
;========================================
; 将 40H~49H 复制到 50H~59H
; 使用: R0(源指针), R1(目的指针), R2(计数)
;========================================
MOVE_BLOCK:
        MOV R0, #40H        ; 源起始地址
        MOV R1, #50H        ; 目的起始地址
        MOV R2, #10         ; 计数器 (10个字节)
LOOP:
        MOV A, @R0          ; 从源地址取数
        MOV @R1, A          ; 存到目的地址
        INC R0              ; 源指针+1
        INC R1              ; 目的指针+1
        DJNZ R2, LOOP       ; 计数器-1，不为0继续
        RET
```

### 8.3 求最大值

```assembly
;========================================
; 比较 R3 和 R4，较大值存入 A
;========================================
GET_MAX:
        MOV A, R3           ; 先假设 R3 是最大值
        CJNE A, R4, COMPARE ; 比较
COMPARE:
        JNC DONE            ; 如果 CY=0 (R3>=R4)，跳转
        MOV A, R4           ; 否则 R4 更大
DONE:
        RET
```

### 8.4 精准延时 (双重循环)

```assembly
;========================================
; 延时子程序 (12MHz晶振)
; 外循环 R1 次，内循环 R2 次
; 总周期 ≈ R1 × R2 × 2 × 1μs
;========================================
DELAY:
        MOV R1, #100        ; 外循环次数
OUTER:
        MOV R2, #200        ; 内循环次数
INNER:
        NOP                 ; 1周期
        DJNZ R2, INNER      ; 2周期
        DJNZ R1, OUTER      ; 2周期
        RET
```

### 8.5 MOVC 查表 (段码显示)

```assembly
;========================================
; 数码管段码查表 (共阴极)
; 输入: A = 要显示的数字 (0-9)
; 输出: A = 对应的段码
;========================================
SEG_TABLE:
        DB 3FH, 06H, 5BH, 4FH, 66H  ; 0-4
        DB 6DH, 7DH, 07H, 7FH, 6FH  ; 5-9

GET_SEG:
        MOV DPTR, #SEG_TABLE ; 表首地址
        MOVC A, @A+DPTR      ; 查表
        RET
```

### 8.6 MOVC 查表 (使用 PC)

```assembly
;========================================
; 使用 PC 查表 (注意偏移量计算)
; 输入: A = 索引
; 输出: A = 表中数据
;========================================
GET_DATA:
        INC A               ; 因为 RET 占2字节，需要调整
        INC A
        MOVC A, @A+PC       ; 查表
        RET
TABLE:
        DB 10H, 20H, 30H, 40H, 50H
```

### 8.7 中断服务程序模板

```assembly
;========================================
; 外部中断0 ISR (带现场保护)
;========================================
        ORG 0003H
        LJMP EX0_ISR

EX0_ISR:
        ; 1. 保存现场
        PUSH ACC
        PUSH PSW
        PUSH B
        PUSH DPH
        PUSH DPL

        ; 2. 中断处理
        ; 任务代码...

        ; 3. 恢复现场 (注意顺序相反)
        POP DPL
        POP DPH
        POP B
        POP PSW
        POP ACC

        ; 4. 中断返回
        RETI
```

### 8.8 定时器初始化 (模式1，50ms)

```assembly
;========================================
; 定时器0 初始化 (12MHz, 50ms)
; TMOD = 01H (模式1)
; 初值 = 65536 - 50000 = 15536 = 3CB0H
;========================================
T0_INIT:
        MOV TMOD, #01H      ; T0模式1
        MOV TH0, #3CH       ; 高8位
        MOV TL0, #0B0H      ; 低8位
        SETB ET0             ; 允许T0中断
        SETB EA              ; 开总中断
        SETB TR0             ; 启动T0
        RET
```

### 8.9 串口初始化 (9600波特率)

```assembly
;========================================
; 串口初始化 (11.0592MHz, 9600波特率)
; SCON = 50H (模式1, 允许接收)
; TH1 = 256 - (11059200/384/9600) = FDH
;========================================
UART_INIT:
        MOV TMOD, #20H      ; T1模式2 (自动重装)
        MOV TH1, #0FDH      ; 波特率初值
        MOV TL1, #0FDH
        MOV SCON, #50H      ; 串口模式1, REN=1
        SETB ES              ; 允许串口中断
        SETB EA              ; 开总中断
        SETB TR1             ; 启动T1
        RET
```

### 8.10 延时抖动 (按键消抖)

```assembly
;========================================
; 按键消抖延时 (约10ms @ 12MHz)
;========================================
DEBOUNCE:
        MOV R6, #20
D1:
        MOV R7, #250
D2:
        NOP
        DJNZ R7, D2
        DJNZ R6, D1
        RET
```

### 8.11 BCD 加法

```assembly
;========================================
; 两个 BCD 数相加
; 输入: R2(BCD1), R3(BCD2)
; 输出: A(BCD结果)
;========================================
BCD_ADD:
        MOV A, R2
        ADD A, R3
        DA A                ; BCD调整
        RET
```

### 8.12 多字节加法

```assembly
;========================================
; 双字节加法
; 输入: R0(低), R1(高) + R2(低), R3(高)
; 输出: R4(低), R5(高)
;========================================
WORD_ADD:
        MOV A, R0
        ADD A, R2
        MOV R4, A           ; 低字节结果
        MOV A, R1
        ADDC A, R3          ; 带进位加高字节
        MOV R5, A           ; 高字节结果
        RET
```

---

## 9. 考试重点

### 9.1 标志位 (PSW)

| 位 | 名称 | 说明 |
|:---:|:---|:---|
| CY | 进位标志 | 加法有进位/减法有借位时置1 |
| AC | 辅助进位 | 低4位向高4位有进位时置1 |
| OV | 溢出标志 | 有符号数运算结果超出范围时置1 |
| P | 奇偶标志 | A中1的个数为奇数时置1 (硬件自动) |

### 9.2 OV 溢出判断

```assembly
; 溢出 = 最高位进位 XOR 次高位进位
MOV A, #7FH    ; 0111 1111 (+127)
ADD A, #01H    ; 结果: 1000 0000 (-128)
               ; OV=1 (正数+正数=负数，溢出!)
```

### 9.3 常考数值

| 项目 | 值 |
|:---|:---|
| 复位后 SP | 07H |
| 第一个 PUSH 地址 | 08H |
| ALE 频率 | 晶振的 1/6 |
| 复位后 P0-P3 | FFH |
| 复位后 A | 00H |
| 复位后 PSW | 00H |

### 9.4 中断优先级 (自然优先级)

```
外部中断0 > 定时器0 > 外部中断1 > 定时器1 > 串口
  (0003H)    (000BH)    (0013H)    (001BH)   (0023H)
```

### 9.5 定时器初值计算

- 机器周期 = 12 / fosc
- 计数值 = 定时时间 / 机器周期
- 初值 = 2^位数 - 计数值

**注意**： 一定要统一单位量级

- 例: 12MHz, 50ms, 16位  
        机器周期 = 12 / 12MHz = 1μs  
        计数值 = 50ms / 1μs = 50000  
        初值 = 65536 - 50000 = 15536 = 3CB0H  

### 9.6 串口波特率计算

- T1模式2 (自动重装):

```asm
TH1 = 256 - (fosc / (384 × 波特率))   ; SMOD=0
TH1 = 256 - (fosc / (192 × 波特率))   ; SMOD=1
```


例: 11.0592MHz, 9600, SMOD=0
```asm
TH1 = 256 - (11059200 / 384 / 9600) = 256 - 3 = 253 = FDH
```

### 9.7 DJNZ 陷阱

```assembly
MOV R2, #0      ; 初值为0
DJNZ R2, $      ; R2 = 0-1 = FFH (255), 然后判断 FFH≠0
                ; 实际循环 256 次！
```

### 9.8 CJNE 比较逻辑

```assembly
CJNE A, #50H, NOT_EQUAL
; 如果 A = 50H: 不跳转，CY=0
; 如果 A < 50H: 跳转，CY=1
; 如果 A > 50H: 跳转，CY=0
NOT_EQUAL:
JC A_LESS       ; CY=1 表示 A < 50H
; 否则 A > 50H
```

---

## 附录：指令周期速查

| 指令类型 | 典型周期 |
|:---|:---:|
| 寄存器操作 (MOV Rn, ...) | 1 |
| 直接地址操作 (MOV direct, ...) | 1-2 |
| 间接地址操作 (MOV @Ri, ...) | 1 |
| 算术运算 (ADD, SUBB, ...) | 1 |
| 逻辑运算 (ANL, ORL, ...) | 1 |
| 跳转指令 (SJMP, AJMP, ...) | 2 |
| 条件跳转 (JZ, JC, JB, ...) | 2 |
| 子程序调用 (LCALL, ACALL) | 2 |
| 返回 (RET, RETI) | 2 |
| 堆栈操作 (PUSH, POP) | 2 |
| 外部RAM操作 (MOVX) | 2 |
| 查表 (MOVC) | 2 |
| 乘除法 (MUL, DIV) | 4 |
| NOP | 1 |

---

*本文档覆盖了 task.md 中 LAB1 的所有汇编考点，配合模板中的 C 语言代码，可应对 8051 期末考试的绝大部分题目。*
