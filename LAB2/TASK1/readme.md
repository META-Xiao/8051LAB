**实验内容**：在同一个 C 文件里定义变量，并使用 `printf` 打印它们的地址或查看 `.m51` 文件。
  - `data`：低 128 字节 RAM（直接寻址，速度最快）。
  - `idata`：全部 256 字节 RAM（必须间接寻址）。
  - `pdata`：外部 RAM 的前 256 字节。
  - `xdata`：全部 64KB 外部存储空间。


## 基本内容

| 关键字 | 物理位置 | 对应汇编指令 | 速度 | 典型用途 |
| :--- | :--- | :--- | :--- | :--- |
| **data** | 内部 RAM 低 128B | `MOV A, direct` | **极快** | 频繁使用的循环变量、标志位。 |
| **idata** | 内部 RAM 256B | `MOV A, @Ri` | **快** | 较大的数组、局部变量。 |
| **pdata** | 外部 RAM 前 256B | `MOVX A, @Ri` | **中** | 外部设备少量的缓冲区。 |
| **xdata** | 外部 RAM 64KB | `MOVX A, @DPTR` | **慢** | 大数据存储、查表。 |



## 代码

```c
#include <reg51.h>
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned int u32;

uchar data v1 = 0x11;
uchar idata v2 = 0x22;
uchar pdata v3 = 0x33;
uchar xdata v4 = 0x44;

void main()
{
	SCON = 0x50;
	TMOD = 0x20;
	TH1 = 0xFD;
	TR1 = 1;
	TI = 1;
	
	printf("data addr: 0x%X, val: 0x%X\n", (u32)&v1, (u32)v1);
	printf("idata addr: 0x%X, val: 0x%X\n", (u32)&v2, (u32)v2);
	printf("pdata addr: 0x%X, val: 0x%X\n", (u32)&v3, (u32)v3);
	printf("xdata addr: 0x%X, val: 0x%X\n", (u32)&v4, (u32)v4);
}
```

在keil内部打开UART通信后输出：

```
data addr: 0x1C, val: 0x11
idata addr: 0x1D, val: 0x22
pdata addr: 0x0, val: 0x33
xdata addr: 0x1, val: 0x44
```