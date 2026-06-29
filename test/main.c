#include "C8051F410.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

unsigned char i;
unsigned char j;
unsigned char counter=0;

sbit I2C_SCL = P0^3;
sbit I2C_SDA = P0^2;
sbit P10=P1^0;
sbit P11=P1^1;
sbit P12=P1^2;
sbit P13=P1^3;

unsigned char code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};

unsigned char AD_CHANNEL;
unsigned int D[4];

float xdata temp;
int xdata temp1;

void PCA_Init()
{
    PCA0MD &= ~0x40;
    PCA0MD  = 0x00;
}

void Timer_Init()
{
    TCON      = 0x40;
    TMOD      = 0x20;
    CKCON     = 0x08;
    TH1       = 0x96;
    TMR2CN    = 0x04;
    TMR2RLL   = 0xF9;
    TMR2RLH   = 0x38;
    TMR2L     = 0xF9;
    TMR2H     = 0x38;
}

void UART_Init()
{
    SCON0     = 0x10;
}

void Port_IO_Init()
{
    XBR0      = 0x01;
    XBR1      = 0x40;
		P1MDOUT |= 0x0F;
	  P1MDOUT |= (1<<7);
}

void Oscillator_Init()
{
    OSCICN    = 0x87;
}

void Interrupts_Init()
{
    IE        = 0xA0;
}

void Init_Device(void)
{
    PCA_Init();
    Timer_Init();
    UART_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}

void delay(int n)
{
	while(n--);
}

//I2C??
void I2C_Delay(void)
{
    unsigned char i;
    for (i = 0; i < 30; i++) {}
}

void I2C_Init(void)
{
    P0MDOUT |= 0x08;
    P0MDOUT &= ~0x04;
    P0MDIN  |= 0x0C;
	
    I2C_SCL = 1;
    I2C_SDA = 1;
}

void I2C_Start(void)
{
    I2C_SDA = 1;
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SDA = 0;
    I2C_Delay();
    I2C_SCL = 0;
}

void I2C_Stop(void)
{
    I2C_SDA = 0;
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SDA = 1;
    I2C_Delay();
}

void I2C_SendByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++) {
        I2C_SDA = (dat & (0x80 >> i)) ? 1 : 0;
        I2C_Delay();
        I2C_SCL = 1;
        I2C_Delay();
        I2C_SCL = 0;
    }
}

unsigned char I2C_ReceiveByte(void)
{
    unsigned char i, dat = 0x00;
    I2C_SDA = 1;
    for (i = 0; i < 8; i++) {
        I2C_SCL = 1;
        I2C_Delay();
        if (I2C_SDA == 1) {
            dat |= (0x80 >> i);
        }
        I2C_SCL = 0;
        I2C_Delay();
    }
    return dat;
}

void I2C_SendAck(unsigned char ack)
{
    I2C_SDA = ack ? 1 : 0;
    I2C_Delay();
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SCL = 0;
}

unsigned char I2C_ReceiveAck(void)
{
    unsigned char ack;
    I2C_SDA = 1;
    I2C_Delay();
    I2C_SCL = 1;
    I2C_Delay();
    ack = I2C_SDA;
    I2C_SCL = 0;
    I2C_Delay();
    return ack;
}

//FCP8591
bit ISendByte(unsigned char sla,unsigned char c)
{
   I2C_Start();  
   I2C_SendByte(sla);          
   I2C_ReceiveAck();
   I2C_SendByte(c);             
   I2C_ReceiveAck();
   I2C_Stop();          
   return(1);
}

unsigned char IRcvByte(unsigned char sla)
{  
	unsigned char c;
  I2C_Start();       
  I2C_SendByte(sla+1);
  I2C_ReceiveAck();
  c=I2C_ReceiveByte();      
  I2C_SendAck(1);         
  I2C_Stop();        
  return(c);
}

//????
void Serial_SendString(char *String)
{
	for (j = 0; String[j] != '\0'; j++)
	{
		TI0 = 0;
    SBUF0 = String[j];
    while(TI0 != 1);
	}
}

void Serial_Printf(char *format, ...)
{
	char String[16];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

void main()
{
   Init_Device();
	 I2C_Init();
	
   while(1)
   {
		 int x=50;
			switch(AD_CHANNEL)
      {
        case 0: ISendByte(0x90,0x41);
                D[0]=IRcvByte(0x90)*2;
		   	   break;  
	    
	      case 1: ISendByte(0x90,0x42);
                D[1]=IRcvByte(0x90)*2; 
		     	 break;  
	      
	      case 2: ISendByte(0x90,0x43);
                D[2]=IRcvByte(0x90)*2;  
		     	 break;  
	      
	      case 3: ISendByte(0x90,0x40);
                D[3]=IRcvByte(0x90)*2; 
		     	 break;  
			}

      if(++AD_CHANNEL>4) AD_CHANNEL=0;
		  
			temp=D[1] * 5.00f / 510 ;
			Serial_Printf("%f\r\n", temp);
			
			temp1=temp * 1000;
			while(x--)
			{
				P10=0;P11=1;P12=1;P13=1;
				P2=led[temp1/1000] & 0x7F;
				delay(3000);
	
				P10=1;P11=0;P12=1;P13=1;
				P2=led[(temp1/100)%10];
				delay(3000);
	
				P10=1;P11=1;P12=0;P13=1;
				P2=led[(temp1/10)%10];
				delay(3000);
	
				P10=1;P11=1;P12=1;P13=0;
				P2=led[temp1%10];
				delay(3000);
			}
   }
}

void t2int02() interrupt 5
{
	TF2H =0;
}

