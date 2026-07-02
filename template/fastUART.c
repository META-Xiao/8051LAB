/*
┌─────────────────────────────────────────┐
│  发送：中断驱动，CPU只负责"喂"数据到队列   │
│  接收：中断驱动，CPU只负责"取"数据出队列   │
│  关键：head/tail 分离读写，避免临界区竞争  │
└─────────────────────────────────────────┘
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ============================================
// 环形缓冲区核心数据结构
// ============================================

#define UART_TX_BUF_SIZE  512   // 发送缓冲，根据突发数据量调整
#define UART_RX_BUF_SIZE  256   // 接收缓冲，根据协议帧长度调整

// 关键：使用 volatile + 内存屏障保证中断可见性
// 实际项目中如果支持 C11，可用 _Atomic 替代
typedef struct {
    volatile uint16_t head;  // 写入位置（中断/主程序写）
    volatile uint16_t tail;  // 读取位置（主程序读）
    uint8_t buffer[UART_TX_BUF_SIZE];
} RingBuffer;

typedef struct {
    volatile uint16_t head;  // 写入位置（中断写）
    volatile uint16_t tail;  // 读取位置（主程序读）
    uint8_t buffer[UART_RX_BUF_SIZE];
} RingBuffer;

// 双缓冲：发送和接收完全分离，避免互相阻塞
static RingBuffer txBuf;
static RingBuffer rxBuf;

// 状态标志
static volatile bool txBusy = false;  // 发送中断是否正在运行

// ============================================
// 环形缓冲区原子操作（平台无关抽象）
// ============================================

// 实际移植时替换为对应平台的内存屏障指令
#define MEMORY_BARRIER()  __asm__ volatile("" ::: "memory")

static inline uint16_t buf_mask(uint16_t idx, uint16_t size) {
    return idx & (size - 1);  // 要求size必须是2的幂次
}

static inline uint16_t buf_count(volatile uint16_t head, volatile uint16_t tail, uint16_t size) {
    return (uint16_t)(head - tail);  // 利用无符号回绕特性
}

// ============================================
// 发送缓冲区操作（主程序调用）
// ============================================

/**
 * @brief 写入发送缓冲，非阻塞，立即返回
 * @param data 数据指针
 * @param len  长度
 * @return 实际写入字节数（可能小于len，表示缓冲满）
 */
uint16_t UART_Write(const uint8_t *data, uint16_t len)
{
    uint16_t written = 0;
    
    // 批量写入，减少中断开关次数
    // 实际项目中这里可能需要关中断或自旋锁
    // DISABLE_INTERRUPTS();
    
    while (written < len) {
        uint16_t nextHead = (txBuf.head + 1) & (UART_TX_BUF_SIZE - 1);
        
        // 缓冲满检查
        if (nextHead == txBuf.tail) {
            break;  // 满，返回已写入数量
        }
        
        txBuf.buffer[txBuf.head] = data[written];
        MEMORY_BARRIER();  // 确保数据写入后再更新head
        txBuf.head = nextHead;
        written++;
    }
    
    // ENABLE_INTERRUPTS();
    
    // 触发发送：如果当前空闲，启动发送中断
    if (!txBusy && written > 0) {
        txBusy = true;
        UART_StartTx();  // 平台相关：写DR寄存器触发TXE中断
    }
    
    return written;
}

/**
 * @brief 格式化输出（类似printf但非阻塞）
 */
uint16_t UART_Printf(const char *fmt, ...)
{
    // 使用栈上临时缓冲，避免动态分配
    char temp[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);
    
    if (len > 0) {
        return UART_Write((uint8_t*)temp, (len < sizeof(temp)) ? len : sizeof(temp));
    }
    return 0;
}

// ============================================
// 接收缓冲区操作（主程序调用）
// ============================================

/**
 * @brief 读取接收缓冲，非阻塞
 * @param data 输出缓冲
 * @param len  最大读取长度
 * @return 实际读取字节数
 */
uint16_t UART_Read(uint8_t *data, uint16_t len)
{
    uint16_t read = 0;
    
    while (read < len) {
        if (rxBuf.head == rxBuf.tail) {
            break;  // 空
        }
        
        data[read] = rxBuf.buffer[rxBuf.tail];
        MEMORY_BARRIER();
        rxBuf.tail = (rxBuf.tail + 1) & (UART_RX_BUF_SIZE - 1);
        read++;
    }
    
    return read;
}

/**
 * @brief 查看接收缓冲数据量（不取出）
 */
uint16_t UART_Available(void)
{
    return buf_count(rxBuf.head, rxBuf.tail, UART_RX_BUF_SIZE);
}

/**
 * @brief 清空接收缓冲
 */
void UART_FlushRx(void)
{
    rxBuf.tail = rxBuf.head;
}

// ============================================
// 中断服务程序（ISR）—— 最关键部分
// ============================================

/**
 * @brief 发送中断（TXE/TC）
 * 触发条件：发送寄存器空，可以写入下一个字节
 */
void UART_TX_ISR(void)
{
    if (txBuf.head != txBuf.tail) {
        // 还有数据，继续发送
        uint8_t data = txBuf.buffer[txBuf.tail];
        txBuf.tail = (txBuf.tail + 1) & (UART_TX_BUF_SIZE - 1);
        MEMORY_BARRIER();
        UART_PutData(data);  // 平台相关：写入DR
    } else {
        // 发送完毕，关闭发送中断（或等待TC后关闭）
        txBusy = false;
        UART_DisableTxInt();  // 平台相关
    }
}

/**
 * @brief 接收中断（RXNE）
 * 触发条件：收到一个字节
 */
void UART_RX_ISR(void)
{
    uint8_t data = UART_GetData();  // 平台相关：读取DR
    uint16_t nextHead = (rxBuf.head + 1) & (UART_RX_BUF_SIZE - 1);
    
    // 溢出处理：覆盖最旧数据（或选择丢弃）
    if (nextHead == rxBuf.tail) {
        // 缓冲满，丢弃最旧数据（tail前进）
        rxBuf.tail = (rxBuf.tail + 1) & (UART_RX_BUF_SIZE - 1);
    }
    
    rxBuf.buffer[rxBuf.head] = data;
    MEMORY_BARRIER();
    rxBuf.head = nextHead;
}

// ============================================
// 高级特性：流控与帧协议支持
// ============================================

/**
 * @brief 等待发送完成（用于关键数据后同步）
 */
void UART_WaitTxComplete(void)
{
    while (txBusy || txBuf.head != txBuf.tail) {
        // 可加入超时或WFI
    }
}

/**
 * @brief 基于空闲中断的帧接收（更高效）
 * 适用于Modbus/自定义协议等帧格式
 */
void UART_IDLE_ISR(void)
{
    // 空闲中断表示一帧接收完毕
    uint16_t frameLen = UART_Available();
    // 设置标志或信号量，通知主程序处理
    frameReady = true;
}

// ============================================
// 平台抽象层（需要移植的部分）
// ============================================

// 以下函数需要根据具体MCU实现：
extern void UART_StartTx(void);      // 启动发送（如写DR+开TXEIE）
extern void UART_DisableTxInt(void); // 关闭发送中断
extern void UART_PutData(uint8_t d); // 写数据寄存器
extern uint8_t UART_GetData(void);   // 读数据寄存器