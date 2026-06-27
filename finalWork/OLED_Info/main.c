/**
 * OLED_Info — 班级学号 (8px间距, 紧凑显示)
 */
#include "sysinit.h"
#include "oled7pin.h"

enum {
    CN_2=0, CN_4=1, CN_I=2, CN_C=3, CN_1=4, CN_SP=5,
    CN_2b=6, CN_4b=7, CN_2c=8, CN_1b=9, CN_1c=10,
    CN_7=11, CN_0=12, CN_5=13, CN_1d=14, CN_3=15, CN_5b=16
};

void main(void)
{
    sysInit();
    oledInit();

    /* 第1行: 班级 24IC1 (5字×8px=40px, 居中 x=44) */
    oledShowChar8(44, 0, CN_2);
    oledShowChar8(52, 0, CN_4);
    oledShowChar8(60, 0, CN_I);
    oledShowChar8(68, 0, CN_C);
    oledShowChar8(76, 0, CN_1);

    /* 第2行: ASCII标签 */
    oledShowStr(28, 2, "Class: 24IC1");

    /* 第3行: 学号 24211705135 (11字×8px=88px, 居中 x=20) */
    oledShowChar8(20, 4, CN_2b);
    oledShowChar8(28, 4, CN_4b);
    oledShowChar8(36, 4, CN_2c);
    oledShowChar8(44, 4, CN_1b);
    oledShowChar8(52, 4, CN_1c);
    oledShowChar8(60, 4, CN_7);
    oledShowChar8(68, 4, CN_0);
    oledShowChar8(76, 4, CN_5);
    oledShowChar8(84, 4, CN_1d);
    oledShowChar8(92, 4, CN_3);
    oledShowChar8(100,4, CN_5b);

    while (1);
}
