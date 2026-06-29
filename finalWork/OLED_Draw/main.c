/**
 * OLED_Draw — 3D 旋转立方体线框
 *
 * CUBE_STATIC: 1=静态第一帧  0=24帧循环动画
 * Keil: 添加 oled7pin.c, 包含路径 ../lib
 */
#define CUBE_STATIC 0   /* 改0为动态旋转 */

#include "config.h"
#include "cube_bmp.h"

#if CUBE_STATIC
  void main(void)
  {
      sysInit();
      oledInit();
      oledDrawBMP(0, 0, 128, 8, cubeFrames[0]);
      while (1);
  }
#else
  static void cubeDelay(void)
  {
      uint i;
      for (i = 0; i < 45000; i++);
  }

  void main(void)
  {
      uchar f = 0;
      sysInit();
      oledInit();

      while (1) {
          oledDrawBMP(0, 0, 128, 8, cubeFrames[f]);
          cubeDelay();
          f++;
          if (f >= CUBE_FRAMES) f = 0;
      }
  }
#endif
