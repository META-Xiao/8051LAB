# 8051 输入输出流指南 (printf / scanf / getchar)

Keil C51 通过 `putchar()` 和 `_getkey()` 两个底层函数实现所有格式化 I/O。只要 UART 配置好，`printf` / `scanf` / `getchar` 即可直接使用。

```
  printf("...")  ──→  putchar(c)  ──→  SBUF  ──→  TX 引脚
  scanf("...")   ──→  _getkey()   ──→  SBUF  ←──  RX 引脚
  getchar()      ──→  _getkey()
```

## 底层函数重定向

```c
// Keil C51 自动将 printf/scanf 重定向到 UART
// 你只需实现 putchar() 和 _getkey()（或使用标准库自动绑定）

// putchar — printf 的底层输出函数
char putchar(char c) {
    SBUF = c;
    while (!TI);    // 等待发送完成
    TI = 0;         // 手动清零 TI
    return c;
}

// _getkey — scanf/getchar 的底层输入函数
char _getkey(void) {
    char c;
    while (!RI);    // 等待接收完成
    RI = 0;         // 手动清零 RI
    c = SBUF;       // 读取缓冲区
    return c;
}
```

---

## printf 格式化输出

### 格式符语法

```
%[标志][宽度][.精度][长度修饰符]类型
 │      │     │      │          │
 │      │     │      │          └── d, u, x, f, s, c ...
 │      │     │      └─────────────── h(16位), l(32位), L(64位)
 │      │     └────────────────────── . + 数字 = 精度（小数位数）
 │      └──────────────────────────── 数字 = 最小宽度
 └─────────────────────────────────── -, +, 0, 空格, #

```

### 数据类型与格式符

Keil C51 中 `int` 为 **16 位**，`long` 为 **32 位**，与现代平台不同。

| 数据类型 | 大小 | 格式符 | 示例 |
|---------|------|--------|------|
| `char` | 8 bit | `%c`（字符） | `printf("%c", 'A');` → `A` |
| `unsigned char` | 8 bit | `%bu`（十进制） `%bx`/`%Bx`（十六进制） | `printf("%bu", 255);` → `255` |
| `int` | 16 bit | `%d`（有符号） `%u`（无符号） | `printf("%d", -100);` → `-100` |
| `unsigned int` | 16 bit | `%u` `%X`/`%x`（十六进制） | `printf("%u", 65535);` → `65535` |
| `long` | 32 bit | `%ld`（有符号） `%lu`（无符号） | `printf("%lu", 100000UL);` → `100000` |
| `unsigned long` | 32 bit | `%lu` `%lX` | `printf("%lX", 0xDEADBEEFUL);` → `DEADBEEF` |
| `bit` | 1 bit | **不能直接 printf** | — |
| `指针` (`xdata *`) | 16 bit | `%A51F` | `printf("%A51F\n", ptr);` → `X:1234` |

### ⚠️ unsigned char 必须用 `%bd` / `%bu` / `%bx`

Keil C51 中 `%d` 默认解析为 16 位，直接用在 `unsigned char` 上会出错：

```c
unsigned char val = 255;

printf("%d\n",  val);   // ⚠️ 可能输出 -1（按 16 位有符号解析）
printf("%bd\n", val);   // 正确输出 255
printf("%bu\n", val);   // 正确输出 255
printf("%bx\n", val);   // 正确输出 FF
```

### 格式修饰符

```c
unsigned int ui = 255;

printf("[%04X]\n", ui);   // [00FF]    补零到 4 位
printf("[%4X]\n",  ui);   // [  FF]    补空格到 4 位
printf("[%08lu]\n", ul);   // [00100000] 补零到 8 位

printf("[%-10s]\n", "hi"); // [hi        ]  左对齐，右边补空格
printf("[%10s]\n",  "hi"); // [        hi]  右对齐，左边补空格

printf("[%+d]\n", 42);    // [+42]     正数显示 + 号

printf("[%02X]\n", uc);   // [0F]      大写
printf("[%02x]\n", uc);   // [0f]      小写
```

### 常用格式模板

```c
unsigned char  sensor = 0xA5;
unsigned int   adc = 1023;
unsigned long  baud = 9600;

printf("Sensor:  %02BX\n", sensor);     // Sensor:  A5
printf("ADC:     %04XH\n", adc);        // ADC:     03FFH
printf("ADC:     %u\n", adc);           // ADC:     1023
printf("Baud:    %lu\n", baud);         // Baud:    9600
printf("[%02d:%02d:%02d]\n", h, m, s); // [09:05:30]
```

---

## scanf 格式化输入

### 基本用法

```c
int val;
scanf("%d", &val);        // 读取十进制整数
```

### 数据类型与格式符

| 数据类型 | 格式符 | 示例 |
|---------|--------|------|
| `char` | `%c` | `scanf("%c", &c);` |
| `unsigned char` | `%bd` | `scanf("%bd", &uc);` |
| `int` | `%d` | `scanf("%d", &i);` |
| `unsigned int` | `%u` | `scanf("%u", &ui);` |
| `long` | `%ld` | `scanf("%ld", &l);` |
| `unsigned long` | `%lu` | `scanf("%lu", &ul);` |
| 十六进制 | `%x` | `scanf("%x", &hex_val);` |

### scanf 的注意事项

```c
// 1. 必须传地址（加 &）
int val;
scanf("%d", &val);    // 正确
scanf("%d", val);     // 崩溃！传的是值不是地址

// 2. unsigned char 必须用 %bd
unsigned char uc;
scanf("%bu", &uc);    // 正确
scanf("%d", &uc);     // 可能溢出（按 16 位写入 8 位变量）

// 3. 缓冲区问题 — scanf 默认会阻塞等待输入
//    如果 UART 没有数据，程序会卡住
```

### 手动解析字符输入（推荐）

在 8051 上 `scanf` 比较笨重，实际项目中更常用 `getchar()` 手动解析：

```c
// 读取一个整数（逐字符解析）
unsigned int read_uint(void) {
    unsigned int result = 0;
    char c;
    while (1) {
        c = getchar();
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        } else {
            break;  // 非数字字符结束输入
        }
    }
    return result;
}

// 用法
unsigned int val = read_uint();
printf("You entered: %u\n", val);
```

### 逐字符输入函数

| 函数 | 说明 |
|------|------|
| `getchar()` | 读取一个字符（ASCII），遇换行返回 `\n` |
| `_getkey()` | 读取一个字符（原始值，不经缓冲区处理） |
| `scanf()` | 格式化输入，自动解析数字/字符串 |

```c
// 读取单个字符
char c = getchar();     // 等待输入一个字符
char c = _getkey();     // 同上，但跳过格式处理

// 读取字符串
char buf[20];
scanf("%s", buf);       // 读取到空格/换行为止

// 读取一行（实用函数）
void read_line(char *buf, unsigned char max_len) {
    unsigned char i = 0;
    char c;
    while (i < max_len - 1) {
        c = getchar();
        if (c == '\r' || c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';  // 字符串结尾
}
```

---

## printf / scanf 速查表

| 函数 | 方向 | 底层调用 |
|:---:|:---:|:---:|
| `printf` | 输出 | `putchar()` |
| `scanf` | 输入 | `_getkey()` |
| `getchar` | 输入 | `_getkey()` |

### printf 格式符速查

| 格式 | 含义 |
|------|------|
| `%d` | int 十进制（16 位有符号） |
| `%u` | unsigned int 十进制（16 位无符号） |
| `%ld` | long 十进制（32 位有符号） |
| `%lu` | unsigned long 十进制（32 位无符号） |
| `%x` / `%X` | 十六进制（小写 / 大写） |
| `%bd` | unsigned char 十进制 |
| `%bu` | unsigned char 无符号十进制 |
| `%bx` / `%Bx` | unsigned char 十六进制（小写 / 大写） |
| `%c` | 单个字符 |
| `%s` | 字符串 |
| `%f` | 浮点数（**8051 禁用**，占 1~2 KB ROM） |
| `%A51F` | 指针地址（`xdata *`，输出格式如 `X:1234`） |
| `%02BX` | 补零 2 位大写 hex |
| `%04X` | 补零 4 位大写 hex |
| `%5d` | 右对齐占 5 位十进制 |
| `%-10s` | 左对齐占 10 位字符串 |
| `%+d` | 正数带 + 号 |

---

## 8051 特殊注意事项

### 1. printf 会占用大量 ROM

Keil C51 的 printf 是完整实现，即使不使用浮点也会占用约 **1 KB ROM**。如果 ROM 紧张：

```c
// 方法 1：使用精简版 printf
// 在 project options → Target → 勾选 "Use MicroLIB"

// 方法 2：自己写精简输出函数
void print_hex(unsigned char val) {
    unsigned char hi = val >> 4;
    unsigned char lo = val & 0x0F;
    putchar(hi > 9 ? 'A' + hi - 10 : '0' + hi);
    putchar(lo > 9 ? 'A' + lo - 10 : '0' + lo);
}
```

### 2. 不要输出 float

浮点库会额外占用 **1~2 KB ROM**，在 8051 上极其奢侈：

- ❌ 不要用
```c
float voltage = 3.14;
printf("Voltage: %f V\n", voltage);
```

- ✅ 用整数定点数代替
```
unsigned int voltage_mv = 3300;
printf("Voltage: %u.%03u V\n",
       voltage_mv / 1000,
       voltage_mv % 1000);       // 输出: Voltage: 3.300 V
```

### 3. scanf 默认阻塞

`scanf` 会一直等待输入，如果 UART 没有数据程序会卡住。建议用 `getchar()` 手动解析。

### 4. TI / RI 必须手动清零

```c
// 发送
SBUF = c;
while (!TI);   // 等待发送完成
TI = 0;        // ⚠️ 必须手动清零！

// 接收
while (!RI);   // 等待接收完成
RI = 0;        // ⚠️ 必须手动清零！
c = SBUF;
```
