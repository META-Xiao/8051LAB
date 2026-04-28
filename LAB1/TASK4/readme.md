**实验内容**：利用判跳指令实现常见的逻辑结构。

1.  **数据块搬运（单层循环）**：将内部 RAM 的 **40H~49H**（共 10 个字节）复制到 **50H~59H**。要求使用 **R0**、**R1** 作为地址指针，**R2** 作为循环计数器（赋初值 10），结合 `DJNZ` 指令实现。
2.  **条件分支（寻找最大值）**：比较 **R3** 和 **R4** 的大小。如果 **R3 > R4**，将 **R3** 的值存入 **A**；否则将 **R4** 存入 **A**。提示：使用 `CJNE` 指令比较，并通过判断 **CY** 标志位来决定跳转。
3.  **精准延时（嵌套循环）**：编写一个双重 `DJNZ` 嵌套循环子程序。

**考试细节（填空/改错题陷阱）**：
-  `DJNZ` 指令的执行逻辑是**先减 1，再判断是否为 0**。如果给寄存器赋初值为 **0**，执行 `DJNZ` 时会先减成 **FFH** (即 255)，从而实际循环了 256 次。
- `CJNE` 指令在比较不相等时发生跳转，同时相当于做了一次减法（目标操作数 - 源操作数）。如果 目标操作数 < 源操作数，硬件会自动将 **CY** 置 **1**，这是实现比较大小的唯一标准手段。

## 基本循环语句

> 汇编中并没有像C那样的循环语句，这里提到的循环子程序都是使用**跳转+标号**的方式得到的

### 无条件跳转

`SJMP` 是 *Short Jump* 即短跳转，直接进入这个子程序中，语法结构是：

```assembly
{$label} : 
    ;  do something
    SJMP {$label}
```

这个就是一个简单的死循环和 `py` 里面的 `while True:` 一样，会一直循环下去。

### 计数循环

`DJNZ` 是 *Decrement and Jump if Not Zero* 即 **减一后跳转，不为0就跳回，为0就跳出** ，语法结构是：

```assembly
MOV {$reg}, #{$value}

{$label} :
    ;  do something

DJNZ {$reg}, {$label}
```

这个等价于 `for(int $reg = $value; $reg != 0; $reg--){}`，会循环 `$value` 次。

当然这里的 `$reg` 寄存器也可以使用  `$direct` 地址

### 条件跳转

**注意**：这里的条件循环的对象变量都是看寄存器 `A` 的值，如果A符合条件就跳转，否则就跳出子程序

1. `JNZ` 是 *Jump if Not Zero* 即 **不为0就跳转**，语法结构是：

```assembly
{$label} :
    ;  do something 

JNZ {$label}
```

如果子程序内部写个 `DEC {$reg}` ，那就是对于这个寄存器持续减1，直到寄存器为0时跳出循环，等价于 `while($reg--){}`

当然对于一般地就是 `while($reg)` ，当寄存器 `$reg` 不为0时一直循环，如果可以使用 `goto` 那么就是`if (A!=0) goto {$label};`

2. `JZ` 是 *Jump if Zero* 即 **为0就跳转**，同理这个也是和寄存器A的值有关，如果寄存器A的值为0，那么就跳转，否则就跳出子程序，语法结构是：

```assembly
{$label} :
    ;  do something

JZ {$label}
```

等价于 `if (A==0) goto {$label};`

### 位条件跳转

1. `JB` 是 *Jump if Bit Set* 即 **如果某位为1就跳转**，语法结构是：

```assembly
{$label} :
    ;  do something

JB {$bit}, {$label}
```

具体点比如 `P1` 寄存器的低2位（0-base）为1就跳转到`LOOP` ，即 `JB P1.2, LOOP` ，类比C就是 `if ((P1>>2) & 1) goto {$label};`

2. `JNB` 是 *Jump if Bit Not Set* 即 **如果某位为0就跳转**，语法结构是：

```assembly
{$label} :
    ;  do something

JNB {$bit}, {$label}
```

具体点比如 `P1` 寄存器的低2位（0-base）为0就跳转到`LOOP` ，即 `JNB P1.2, LOOP` ，类比C就是 `if (((P1>>2) & 1) == 0) goto {$label};`


3. `JBC` 是 *Jump if Bit Cleared* 即 **如果某位为0就跳转，并清除该位**，语法结构是：

```assembly
{$label} :
    ;  do something

JBC {$bit}, {$label}
```

具体点比如 `P1` 寄存器的低2位（0-base）为0就跳转到`LOOP` ，并清除该位，即 `JBC P1.2, LOOP`

这个实际上就是个 `if ((P1>>2)&1 == 0) {goto {$label}; P1 &= ~(1<<2);}`


### 比较跳转

`CJNE` 是 *Compare and Jump if Not Equal* 即 **比较并跳转，不相等就跳转**，语法结构是：

```assembly
CJNE {$reg}, {$value}, {$label}
```

具体点比如 `CJNE A, #0x10, LOOP` 就是如果 `A` 不等于 `0x10` 就跳转到 `LOOP`，等价于 `if (A != 0x10) goto {$label};`

### 代码

1. **数据块搬运（单层循环）**

```assembly
MOV R0, #40H
MOV R1, #50H 
MOV R2, #0AH

LOOP:
    MOV A, @R0
    MOV @R1, A
    INC R0
    INC R1
    DJNZ R2, LOOP

END
```

2. **条件分支（寻找最大值）**