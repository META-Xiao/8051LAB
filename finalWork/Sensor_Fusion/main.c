/**
 * sensorFusion — 多传感器融合
 *
 * Key2: 切换 超声波->电压->温度   Key1: 温度下切换 ℃/℉
 */
#include "config.h"

/* ===== 状态 ===== */
#define STATE_BOOT  0
#define STATE_ULTRA 1
#define STATE_ADC   2
#define STATE_TEMP  3

static uchar code keyTable[16] = {
    0x77,0xB7,0xD7,0xE7, 0x7B,0xBB,0xDB,0xEB,
    0x7D,0xBD,0xDD,0xED, 0x7E,0xBE,0xDE,0xEE
};

static uchar keyScan(void)
{
    uchar s1, s2, kc, j;
    P2 = 0xF0; s1 = P2;
    if ((s1 & 0xF0) == 0xF0) return 0;
    { uint d; for (d = 0; d < 6000; d++); }
    P2 = 0xF0; s1 = P2;
    if ((s1 & 0xF0) == 0xF0) return 0;
    P2 = 0x0F; s2 = P2;
    kc = s1 | s2;
    for (j = 0; j < 16; j++)
        if (kc == keyTable[j]) return j + 1;
    return 0;
}

static void delayShort(void) { uint i; for (i = 0; i < 30000; i++); }

/** 延时并轮询所有按键, 返回按键值(0=超时) */
static uchar delayPoll(uchar n)
{
    uchar k;
    while (n--) {
        delayShort();
        k = keyScan();
        if (k) return k;
    }
    return 0;
}

static void oledShowFloat(uchar x, uchar y, uint val)
{
    uchar buf[7];
    buf[0] = '0' + (val / 1000) % 10;
    buf[1] = '0' + (val / 100) % 10;
    buf[2] = '.';
    buf[3] = '0' + (val / 10) % 10;
    buf[4] = '0' + val % 10;
    buf[5] = 0;
    oledShowStr(x, y, buf);
}

/**
 * 开机动画
 */
static void bootAnim(void)
{
    uchar code *msg = "Welcome!";
    uchar i, buf[10], j;

    for (i = 0; msg[i]; i++) {
        buf[i]   = msg[i];
        buf[i+1] = 0;
        oledShowStr(32, 3, buf);
        for ( j = 0; j<10; j++)delayShort();
    }
    delayShort();
    delayShort();
}

void main(void)
{
    uchar state  = STATE_BOOT;
    uchar lastKey = 0, key, fahrMode = 0, refresh = 1;
    uint  dist, volt, temp, lastDist = 0xFFFF, lastVolt = 0xFFFF, lastTemp = 0xFFFF;

    sysInit();
    oledInit();
    buzzerInit();
    ultraInit();
    ds18b20Init();
    pcf8591Init();
    P0MDOUT |= 0x40; 
    P2MDOUT  = 0x0F; P2 = 0xFF;

    /* ---- 开机动画 ---- */
    bootAnim();
    oledClear();
    state   = STATE_ULTRA;
    refresh = 1;

    while (1)
    {
        key = keyScan();
        if (key && key != lastKey) {
            if (key == 2) {
                state++;
                if (state > STATE_TEMP) state = STATE_ULTRA;
                refresh   = 1;
            }
            if (key == 1 && state == STATE_TEMP) {
                fahrMode = !fahrMode;
                refresh  = 1;
            }
            lastKey = key;
            while (keyScan()) {}  /* 等松开 */
            lastKey = 0;          /* 复位, 允许下次再按 */
        }

        switch (state) {

        case STATE_ULTRA:
            dist = ultraRead();
            if (dist != lastDist) { refresh = 1; lastDist = dist; }
            if (refresh) {
                oledClear();
                oledShowStr(0, 0, "Distance:");
                if (dist < 400) {
                    oledShowNum(0, 2, dist, 3, 16);
                    oledShowStr(32, 2, "cm");
                } else {
                    oledShowStr(0, 2, "Out of range");
                }
                refresh = 0;
            }
            /* <80cm 嘀嘀声, 距离越近越急促 */
            if (dist < 80) {
                uchar n;
                buzzerBeep(2500, 40);
                for (n = dist / 4 + 1; n; n--) delayPoll(1);
            } else {
                delayPoll(3);
            }
            break;

        case STATE_ADC:
            volt = (uint)pcf8591ReadADC(1) * 5000UL / 255UL;
            if (volt != lastVolt) { refresh = 1; lastVolt = volt; }
            if (refresh) {
                oledClear();
                oledShowStr(0, 0, "ADC Voltage:");
                oledShowFloat(0, 2, volt / 10);
                oledShowStr(56, 2, "V");
                refresh = 0;
            }
            delayPoll(3);
            break;

        case STATE_TEMP:
            ds18b20Start();
            if (delayPoll(20) == 2) {
                state   = STATE_ULTRA;
                refresh = 1;
                lastKey = 2;
                while (keyScan()) {}  /* 等松开, 防主循环重复处理 */
                lastKey = 0;
                break;
            }
            temp = ds18b20Read();

            if (temp > 3200) {
                buzzerBeep(2500, 60);   /* 嘀 */
                { uint d; for(d=0;d<12000;d++); }
                buzzerBeep(2500, 60);   /* 嘀 */
                delayPoll(2);
            }

            if (fahrMode)
                temp = (uint)((ulong)temp * 9UL / 5UL) + 3200UL;
            if (temp != lastTemp) { refresh = 1; lastTemp = temp; }
            if (refresh) {
                oledClear();
                oledShowStr(0, 0, fahrMode ? "Temp(F):" : "Temp(C):");
                oledShowFloat(0, 2, temp);
                refresh = 0;
            }
            delayPoll(3);
            break;
        }
    }
}
