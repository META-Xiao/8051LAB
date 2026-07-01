## 常用通信协议

### UART 串口通信

UART的具体教程推荐阅读[8051 Microcontroller UART (Serial Communication) | Everything You Need to Know](https://junctionbyte.com/8051-microcontroller-uart/)

值得注意的是，只要UART配置好了，keil自动重载了`printf`,`scanf`,`getchar`函数，因此不需要手动重载。

| 函数 | 方向 | 底层调用 |
| :---: | :---: | :---: |
| `printf` | 串口->发 | `putchar()` |
| `scanf` | 串口->收 | `_getkey()` |
| `getchar` | 串口->收 | `_getkey()` |

波特率计算公式，这里以T1M2（自动重装载）为例

$$
\text{Baud} = \frac{2^{SMOD}}{32} \cdot \frac{\text{FOSC}}{12 \cdot (256-TH1)}
$$

SMOD=0：

$$
\text{Baud} = \frac{\text{FOSC}}{12 \cdot 32 \cdot (256-TH1)} = \frac{\text{FOSC}}{384 \cdot (256-TH1)}
$$

SMOD=1，波特率加倍：

$$
\text{Baud} =  2 \frac{\text{FOSC}}{12 \cdot 32 \cdot (256-TH1)}  = \frac{\text{FOSC}}{192 \cdot (256-TH1)}
$$


具体代码参考 `template\UART.c`

### I2C 通信

I2C的具体教程推荐阅读[8051 Microcontroller I2C Guide | 8051 I2C Driver & PCF8574 IO Module](https://junctionbyte.com/8051-microcontroller-i2c/)

这里配置IIC时需要软件写个延迟函数（最好5us），这里需要根据FOSC计算需要的机器周期，参考`template\IIC.c`

#### I2C 时序文字表

| 动作 | SDA 要求 | SCL 要求 | 时序步骤（文字描述） |
|:---|:---|:---|:---|
| **START** | SCL 为高期间，SDA 从高变低 | 保持高电平 | ① SDA 置高 → ② SCL 置高 → ③ 延时 → ④ SDA 置低（高→低跳变） → ⑤ 延时 → ⑥ SCL 置低 |
| **STOP** | SCL 为高期间，SDA 从低变高 | 保持高电平 | ① SDA 置低 → ② SCL 置高 → ③ 延时 → ④ SDA 置高（低→高跳变） → ⑤ 延时 |
| **发送 1 位** | SCL=0 时设置电平，SCL=1 期间必须稳定 | 低→高→低 | ① SCL 置低 → ② SDA 置目标位 → ③ 延时 → ④ SCL 置高 → ⑤ 延时 → ⑥ SCL 置低 |
| **接收 1 位** | 主机释放 SDA，由从机驱动 | 低→高→低 | ① SCL 置低 → ② SDA 置高（释放总线） → ③ 延时 → ④ SCL 置高 → ⑤ 读取 SDA 电平 → ⑥ 延时 → ⑦ SCL 置低 |
| **接收 ACK** | 主机释放 SDA，从机在第 9 个时钟拉低 | 第 9 个时钟：低→高→低 | ① SDA 置高（释放） → ② 延时 → ③ SCL 置高 → ④ 延时 → ⑤ 读取 SDA（0=ACK，1=NAK） → ⑥ SCL 置低 → ⑦ 延时 |
| **发送 ACK** | 主机在 SCL=0 时设置 ACK 电平 | 低→高→低 | ① SCL 置低 → ② SDA 置 ACK 值（0 或 1） → ③ 延时 → ④ SCL 置高 → ⑤ 延时 → ⑥ SCL 置低 |

---

#### 完整写寄存器流程表

| 步骤 | 动作 | 方向 | 说明 |
|:---|:---|:---|:---|
| 1 | START | 主机→总线 | 发起通信 |
| 2 | 发送从机地址+写位 | 主机→从机 | 7 位地址 + 1 位写标志（0） |
| 3 | 接收 ACK | 从机→主机 | 第 9 个时钟，从机拉低 SDA |
| 4 | 发送寄存器地址 | 主机→从机 | 8 位地址 |
| 5 | 接收 ACK | 从机→主机 | 第 9 个时钟 |
| 6 | 发送数据 | 主机→从机 | 8 位数据 |
| 7 | 接收 ACK | 从机→主机 | 第 9 个时钟 |
| 8 | STOP | 主机→总线 | 结束通信 |

---

#### 读寄存器流程表

| 步骤 | 动作 | 方向 | 说明 |
|:---|:---|:---|:---|
| 1 | START | 主机→总线 | 发起通信 |
| 2 | 发送从机地址+写位 | 主机→从机 | 先写寄存器地址 |
| 3 | 接收 ACK | 从机→主机 | |
| 4 | 发送寄存器地址 | 主机→从机 | 指定要读的寄存器 |
| 5 | 接收 ACK | 从机→主机 | |
| 6 | 重复 START | 主机→总线 | 重新发起 |
| 7 | 发送从机地址+读位 | 主机→从机 | 7 位地址 + 1 位读标志（1） |
| 8 | 接收 ACK | 从机→主机 | |
| 9 | 接收数据 | 从机→主机 | 8 位数据 |
| 10 | 发送 ACK/NAK | 主机→从机 | 0=继续读，1=停止读 |
| 11 | STOP | 主机→总线 | 结束通信 |

### SPI 通信

剩下这些通信我们考试不怎么考，所以这里就不写了，除非我哪天需要学到这些通信方法，再补上。

这里安利一个很好的网站[JunctionByte](https://junctionbyte.com/category/learn/)，里面有很多嵌入式的教程，配图也很不错，非常适合初学者上手辅导。