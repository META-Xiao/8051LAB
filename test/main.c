#include "C8051F410.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

unsigned char sendata[]={"0000,0000,0000\r\n"};
unsigned char i;
unsigned char j;
unsigned char counter=0;
sbit I2C_SCL = P0^3;
sbit I2C_SDA = P0^2;

int xdata AX, AY, AZ, GX, GY, GZ;
float xdata AngleAccXZ=0, AngleGyroXZ=0, AngleXZ=0;
float xdata AngleAccYZ=0, AngleGyroYZ=0, AngleYZ=0;
float xdata AngleAccYX=0, AngleGyroYX=0, AngleYX=0;
float xdata a, b, c;
float xdata Alpha=0.001;

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

#define MPU6050_SMPLRT_DIV      0x19
#define MPU6050_CONFIG          0x1A
#define MPU6050_GYRO_CONFIG     0x1B
#define MPU6050_ACCEL_CONFIG    0x1C
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_ACCEL_XOUT_L    0x3C
#define MPU6050_ACCEL_YOUT_H    0x3D
#define MPU6050_ACCEL_YOUT_L    0x3E
#define MPU6050_ACCEL_ZOUT_H    0x3F
#define MPU6050_ACCEL_ZOUT_L    0x40
#define MPU6050_TEMP_OUT_H      0x41
#define MPU6050_TEMP_OUT_L      0x42
#define MPU6050_GYRO_XOUT_H     0x43
#define MPU6050_GYRO_XOUT_L     0x44
#define MPU6050_GYRO_YOUT_H     0x45
#define MPU6050_GYRO_YOUT_L     0x46
#define MPU6050_GYRO_ZOUT_H     0x47
#define MPU6050_GYRO_ZOUT_L     0x48
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_PWR_MGMT_2      0x6C
#define MPU6050_WHO_AM_I        0x75
#define MPU6050_ADDRESS         0xD0

void MPU6050_WriteReg(unsigned char RegAddress, unsigned char Data)
{
    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_ReceiveAck();
    I2C_SendByte(RegAddress);
    I2C_ReceiveAck();
    I2C_SendByte(Data);
    I2C_ReceiveAck();
    I2C_Stop();
}

void MPU6050_ReadRegs(unsigned char RegAddress, unsigned char *DataArray, unsigned char Count)
{
    unsigned char i;

    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_ReceiveAck();
    I2C_SendByte(RegAddress);
    I2C_ReceiveAck();

    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS | 0x01);
    I2C_ReceiveAck();
    for (i = 0; i < Count; i++) {
        DataArray[i] = I2C_ReceiveByte();
        if (i < Count - 1) {
            I2C_SendAck(0);
        } else {
            I2C_SendAck(1);
        }
    }
    I2C_Stop();
}

void MPU6050_Init(void)
{
    I2C_Init();
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

void MPU6050_GetData(int *AccX, int *AccY, int *AccZ, int *GyroX, int *GyroY, int *GyroZ)
{
    static xdata unsigned char Data[14];

    MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, Data, 14);

    *AccX  = ((int)Data[0]  << 8) | Data[1];
    *AccY  = ((int)Data[2]  << 8) | Data[3];
    *AccZ  = ((int)Data[4]  << 8) | Data[5];
    *GyroX = ((int)Data[8]  << 8) | Data[9];
    *GyroY = ((int)Data[10] << 8) | Data[11];
    *GyroZ = ((int)Data[12] << 8) | Data[13];
}

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
   MPU6050_Init();
	
   while(1)
   {
	   Serial_Printf("%d,%d,%d\r\n",(int)a,(int)b,(int)c);
   }
}

void t2int02() interrupt 5
{
	TF2H =0;
	
	MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
	
	GX += 19;
	GY += 43;
	GZ += 13;

	AngleAccXZ  = -atan2(AX, AZ) / 3.1415926 * 180.0;
	AngleGyroXZ = AngleXZ + (float)GY / 32768.0 * 2000.0 * 0.025;
	AngleXZ     = (Alpha * AngleAccXZ) + ((1.0 - Alpha) * AngleGyroXZ);

	AngleAccYZ  = -atan2(AY, AZ) / 3.1415926 * 180.0;
	AngleGyroYZ = AngleYZ + (float)GX / 32768.0 * 2000.0 * 0.025;
	AngleYZ     = (Alpha * AngleAccYZ) + ((1.0 - Alpha) * AngleGyroYZ);

	AngleAccYX  = -atan2(AY, AX) / 3.1415926 * 180.0;
	AngleGyroYX = AngleYX + (float)GZ / 32768.0 * 2000.0 * 0.025;
	AngleYX     = (Alpha * AngleAccYX) + ((1.0 - Alpha) * AngleGyroYX);	
		
	a = AngleXZ;
	b = AngleYZ;
	c = AngleGyroYX;
}

