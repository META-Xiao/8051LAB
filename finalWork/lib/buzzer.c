#include "buzzer.h"
#include "intrins.h"

#define US_PER_LOOP   4
#define TICK_US       250     /* T1 溢出周期 ≈ 250us              */

/* ===== 非阻塞状态 ===== */
static uint   nbPeriod;       /* 半周期 (tick 数)                 */
static uint   nbOnTicks;      /* 响持续 tick 数                   */
static uint   nbOffTicks;     /* 停持续 tick 数                   */
static uchar  nbCycles;       /* 剩余循环次数                     */
static uint   nbTickCnt;      /* 当前 tick 计数                   */
static uchar  nbPhase;        /* 0=停, 1=响                       */
static uchar  nbActive;       /* 1=鸣叫进行中                     */
static uchar  nbToneCnt;      /* 响阶段内的翻转计数               */

/* ===== 阻塞实现 ===== */

void buzzerInit(void)
{
    P1MDOUT |= 0x80;
    BUZZER_PIN = 0;
}

void buzzerOn(void)  { BUZZER_PIN = 1; }
void buzzerOff(void) { BUZZER_PIN = 0; }
void buzzerToggle(void) { BUZZER_PIN = ~BUZZER_PIN; }

static void delayUs(uint us)
{
    uchar i;
    while (us--) {
        for (i = 0; i < US_PER_LOOP; i++) _nop_();
    }
}

static void delayMs(uint ms)
{
    uchar j;
    while (ms--) {
        for (j = 0; j < 250; j++) _nop_();
    }
}

void buzzerBeep(uint freqHz, uint durMs)
{
    uint halfUs, elapsed, perMs, cyc;

    if (freqHz == 0) {
        buzzerOn();  delayMs(durMs);  buzzerOff();
        return;
    }

    halfUs = 500000UL / freqHz;
    if (halfUs < 10)  halfUs = 10;
    if (halfUs > 25000) halfUs = 25000;

    for (elapsed = 0; elapsed < durMs; elapsed++) {
        perMs = 1000UL / (halfUs * 2);
        if (perMs == 0) perMs = 1;
        for (cyc = 0; cyc < perMs; cyc++) {
            BUZZER_PIN = 1;  delayUs(halfUs);
            BUZZER_PIN = 0;  delayUs(halfUs);
        }
    }
}

void buzzerBeepPattern(uint freqHz, uint onMs, uint offMs, uchar cycles)
{
    uchar i;
    if (freqHz == 0) {
        for (i = 0; i < cycles; i++) {
            buzzerOn();  delayMs(onMs);
            buzzerOff(); delayMs(offMs);
        }
        return;
    }
    for (i = 0; i < cycles; i++) {
        buzzerBeep(freqHz, onMs);
        delayMs(offMs);
    }
}

/* ===== 非阻塞实现 ===== */

void buzzerSetupTimer(void)
{
    TMR2CN   = 0x04;
    TMR2RLL  = 0x02;
    TMR2RLH  = 0xFE;
    TMR2L    = 0x02;
    TMR2H    = 0xFE;
    ET2      = 1;
}

void buzzerStart(uint freqHz, uint onMs, uint offMs, uchar cycles)
{
    uint halfUs;
    if (freqHz == 0) freqHz = 2000;
    halfUs = 500000UL / freqHz;
    nbPeriod  = (halfUs + TICK_US - 1) / TICK_US;
    if (nbPeriod < 1) nbPeriod = 1;

    nbOnTicks  = onMs  * 1000UL / TICK_US;
    nbOffTicks = offMs * 1000UL / TICK_US;
    if (nbOnTicks  < 1) nbOnTicks  = 1;
    if (nbOffTicks < 1) nbOffTicks = 1;

    nbCycles   = cycles;
    nbTickCnt  = 0;
    nbToneCnt  = 0;
    nbPhase    = 1;
    nbActive   = 1;
}

void buzzerStop(void)
{
    nbActive = 0;
    BUZZER_PIN = 0;
}

uchar buzzerBusy(void)
{
    return nbActive;
}

void buzzerTick(void)
{
    if (!nbActive) return;

    nbTickCnt++;

    if (nbPhase) {
        /* 响阶段 */
        nbToneCnt++;
        if (nbToneCnt >= nbPeriod) {
            nbToneCnt = 0;
            buzzerToggle();
        }
        if (nbTickCnt >= nbOnTicks) {
            nbTickCnt = 0;
            nbPhase  = 0;
            BUZZER_PIN = 0;
        }
    } else {
        /* 停阶段 */
        if (nbTickCnt >= nbOffTicks) {
            nbTickCnt = 0;
            nbPhase  = 1;
            nbToneCnt = 0;
            nbCycles--;
            if (nbCycles == 0) {
                nbActive = 0;
                BUZZER_PIN = 0;
            }
        }
    }
}
