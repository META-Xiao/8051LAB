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