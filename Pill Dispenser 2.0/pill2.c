#include <reg52.h>
#include <intrins.h>

#define BOX P3 // MSB Box1 , Box2 LSB

sbit RS = P0^4;
sbit EN = P0^5;
sbit D0 = P2^0;
sbit D1 = P2^1;
sbit D2 = P2^2;
sbit D3 = P2^3;
sbit D4 = P2^4;
sbit D5 = P2^5;
sbit D6 = P2^6;
sbit D7 = P2^7;

sbit SCL = P0^0;
sbit SCK = P0^0;
sbit SDA = P0^1;

sbit sw_1 = P0^2;
sbit sw_2 = P0^3;
sbit ir = P1^0;
sbit buzzer = P1^1;
sbit led = P1^2;

//unsigned char tempchr,mem=0xA0,rtc=0xD0,sec=0x00,min=0x01,hr=0x02,day=0x03;
//int i=0,j=0,fn=0,fn2=0,dat=0,dat_bcd=0,alarms=0,tmpint=0;
int i=0,j=0,dat=0,alarms=0,fn2=0,fn=0,Time_Day=0,Alarm_Day=0,Alarm_Over=0;
//-------------------------------------------------------------------------------------------------

// Define i2c speed
#define I2C_SPEED_FACTOR	1	// Low value means low i2c frequency
#define Crystal_Value		12	// MHz
#define HalfBitDelay 		(500*Crystal_Value)/(12*I2C_SPEED_FACTOR)

// Define macros
#define Set_SDA_Low		SDA = 0
#define Set_SDA_High	SDA = 1
#define Set_SCK_Low		SCK = 0
#define Set_SCK_High	SCK = 1

//Function Declarations
void __delay_us(unsigned int);
void InitI2C(void);
void I2C_Start(void);
void I2C_ReStart(void);
void I2C_Stop(void);
void I2C_Send_ACK(void);
void I2C_Send_NACK(void);
bit	I2C_Write_Byte(unsigned char);
unsigned char I2C_Read_Byte(void);

