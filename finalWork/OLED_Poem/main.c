/**
 * OLED_Poem — OLED显示唐诗 刘禹锡-酬乐天
 */
#include "sysinit.h"
#include "font_cn.h"      /* 先于 oled7pin.h, 定义 HZK_EXTERN_ONLY */
#include "oled7pin.h"

enum {
    CN_CHEN=0, CN_ZHOU, CN_CE, CN_PAN, CN_QIAN, CN_FAN, CN_GUO, CN_DOU,
    CN_BING=8, CN_SHU, CN_QIAN2, CN_TOU, CN_WAN, CN_MU, CN_CHUN, CN_JU
};

void main(void)
{
    sysInit();
    oledInit();

    /* 沉舟侧畔千帆过，*/
    oledShowCn(0,  0, CN_CHEN);
    oledShowCn(16, 0, CN_ZHOU);
    oledShowCn(32, 0, CN_CE);
    oledShowCn(48, 0, CN_PAN);
    oledShowCn(64, 0, CN_QIAN);
    oledShowCn(80, 0, CN_FAN);
    oledShowCn(96, 0, CN_GUO);
    oledShowCn(112,0, CN_DOU);

    /* 病树前头万木春。*/
    oledShowCn(0,  2, CN_BING);
    oledShowCn(16, 2, CN_SHU);
    oledShowCn(32, 2, CN_QIAN2);
    oledShowCn(48, 2, CN_TOU);
    oledShowCn(64, 2, CN_WAN);
    oledShowCn(80, 2, CN_MU);
    oledShowCn(96, 2, CN_CHUN);
    oledShowCn(112,2, CN_JU);

    while (1);
}
