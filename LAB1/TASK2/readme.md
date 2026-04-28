**实验内容**：执行以下运算，并预测执行后 $PSW$ 寄存器的 $CY$（进位）和 $OV$（溢出）位。

1. `MOV A, #0FFH`, `ADD A, #01H`（观察 $CY$）。
2. `MOV A, #7FH`, `ADD A, #01H`（观察 $OV$，思考为什么补码运算溢出了？）。
3. 使用 `MUL AB` 指令计算 $255 \times 2$，观察结果如何分布在 $A$ 和 $B$ 中。

## 基本算数指令

- `ADD` 普通加法，`ADD A, {$OPT}` 即 `A = A + {$OPT}` ，例如：
    - `ADD A, Rn` 将寄存器 `Rn` 的值加到 `A` 中。
    - `ADD A, #data` 将立即数 `data` 加到 `A` 中。
    - `ADD A, @Ri` 将寄存器 `Ri` 指向的内存单元的值加到 `A` 中。
    - `ADD A, direct` 将直接寻址的内存单元的值加到 `A` 中。
- `ADDC` 带进位加法，`ADDC A, {$OPT}` 即 `A = A + {$OPT} + CY` **这里CY是PSW程序状态寄存器的第七位，如果其为1就是产生了进位**
- `SUB` 普通减法，`SUB A, {$OPT}` 即 `A = A - {$OPT}` **8051中没有`SUB`，只有`SUBB`**
- `SUBB` 带借位减法，`SUBB A, {$OPT}` 即 `A = A - {$OPT} - CY`
- `INC` 加一，`INC A` 即 `A = A + 1`
- `DEC` 减一，`DEC A` 即 `A = A - 1`
- `MUL` 乘法，`MUL AB` ，A*B的结果高位存于 `B`，低位存于 `A`
- `DIV` 除法，`DIV AB` ，A/B的结果商存于 `A`，余数存于 `B`
- `DA` 十进制调整，`DA A` 即 `A = A + 06H + CY`，用于BCD码加法运算
- `ANL` 与运算，`ANL A, {$OPT}` 即 `A = A & {$OPT}`
- `ORL` 或运算，`ORL A, {$OPT}` 即 `A = A | {$OPT}`
- `XRL` 异或运算，`XRL A, {$OPT}` 即 `A = A ^ {$OPT}`

## 代码

```asm
MOV A, #0FFH
ADD A, #01H
MOV A, #7FH
ADD A, #01H
MOV A, #0FFH
MOV B, #02H
MUL AB
```