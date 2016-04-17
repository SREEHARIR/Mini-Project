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

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

// Define EEPROM i2c device address
#define Device_Address_EEPROM	0xA0

#define Alarm_Memory 0x00

void Write_Byte_To_EEPROM(unsigned char, unsigned char);
unsigned char Read_Byte_From_EEPROM(unsigned char);
void Write_Bytes_To_EEPROM(unsigned char,unsigned char* ,unsigned char);
void Read_Bytes_From_EEPROM(unsigned char, unsigned char* , unsigned int);
void Get_EEPROM_Alarm(int);
void Add_EEPROM_Alarm(unsigned char*);
void DisplayAlarmToLCD(unsigned char*);
void Delete_EEPROM_Alarm(int);

unsigned char pAlarmArray[6];

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------


// Define DS1307 i2c device address
#define Device_Address_RTC	0xD0

// Define Time Modes
#define AM_Time					0
#define PM_Time					1
#define TwentyFourHoursMode		2

// Define days
#define Sunday		1
#define Monday		2
#define Tuesday		3
#define Wednesday	4
#define Thursday	5
#define Friday		6
#define Saturday	7

// Function Declarations
void Write_Byte_To_RTC(unsigned char, unsigned char);
unsigned char Read_Byte_From_RTC(unsigned char);
void Write_Bytes_To_RTC(unsigned char,unsigned char*,unsigned char);
void Read_Bytes_From_RTC(unsigned char,unsigned char*,unsigned int);
void Set_RTC_Time(unsigned char,unsigned char,unsigned char,unsigned char,unsigned char);
void Get_RTC_Time(void);
void Set_RTC_Date(unsigned char,unsigned char,unsigned char);
void Get_RTC_Date(void);
bit isRTCStopped();
void startRTC();
void stopRTC();

// Global RTC Array and temp variable
unsigned char pRTCArrayTime[5];
unsigned char pRTCArrayDate[3];
unsigned char Temp,Prev_Day;

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

void DisplayTimeToLCD(unsigned char*) ;
void DisplayDateToLCD( unsigned char*);

//----------------------------------------------------------------------------------------------------

/*
fn -> 0-time, 1-set_time, 2-add_alarm(yes/no), 3-remove_alarm(yes/no)
*/
/*
saving in eeprom ->
0x00	 - num of alarms
arr[0] -> 0x01*i - motor 1 rotation & motor 2 rotation
arr[1] -> 0x02*i - min of alarm i
arr[2] -> 0x03*i - hr of alarm i
arr[3] -> 0x04*i - day of alarm i

*/
//char tim[4],timchk[4];
//int timi[4];
/*
tim 		-> 0-sec,		1-min, 2-hr, 3-day //read from rtc
timchk	-> 0-motor, 1-min, 2-hr, 3-day //read from mem
*/

void delay(int i);
void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear();
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init();
void Lcd_Write_Char(char a);
void Lcd_Write_String(char *a);
//void start(void);
//void stop();
//void write(unsigned char dat);
//void ack(void);
//void noack(void);
//unsigned char read();
//void save_i2c(char dev,char addr,char ch);
//char read_i2c(char dev,char addr);
//void time(void);
//void display_time(void);
void check_alarm(void);
//unsigned char Bcd_to_dec (unsigned char val);
//unsigned char Dec_to_bcd (unsigned char val);


// TODO //
//void interpt(void);
void funtions(void);
void motor(int m1,int m2);
//void sort(void);
void alarm(void);
//void display_alarm(int num);
//void delete_alarm(int num);
//void add_alarm(void);
void wipe(void);
//// TODO //



void delay(int i)
{
	int j;
	while(i--)
		for(j=0;j<1000;j++);
}

void wipe(void)
{
for(i=0;i<10;i++){
	Write_Byte_To_EEPROM(i,0x00);
}
}

void check_alarm(void)
{
	alarms=Read_Byte_From_EEPROM(Alarm_Memory);
	//alarms=alarms-48;
	//Lcd_Write_Char(alarms+48);
	for(i=1;i<=alarms;i++)
	{
		Get_EEPROM_Alarm(i);
		//tempchr=((timchk[2]&0xF0)>>4);
		//timchk[2]=timchk[2]&0x0F;
		Alarm_Over = pAlarmArray[0];
		Alarm_Day = pAlarmArray[3];
		Time_Day = pRTCArrayTime[3];
		if((pAlarmArray[1] == pRTCArrayTime[1]) && (pAlarmArray[2] == pRTCArrayTime[2]) && (( Time_Day % Alarm_Day ) == 0) && (Alarm_Over)==0) //check min hr day
		{
			//timchk[3]+=tempchr;
			//tempchr=(tempchr<<4);
			//timchk[3]=(timchk[3]|tempchr);
			Write_Byte_To_EEPROM(Alarm_Memory+1+((i-1)*6),1); // save if alarm over in day register bits 4
			motor(pAlarmArray[4],pAlarmArray[5]);
			alarm();
		}
	}
}

void alarm()
{
	Lcd_Clear();
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String("Take Pill");
	while(1)
	{
		//buzzer=0;
		//led=0;
		if((sw_1==1) || (sw_2==1) )//|| (ir==0))
		{
			buzzer=1;
			led=1;
			while((sw_1==1) || (sw_2==1));
			Lcd_Clear();
			Lcd_Set_Cursor(1,0);
			return;
		}
		Get_RTC_Time();
		DisplayTimeToLCD(pRTCArrayTime);
		if((pRTCArrayTime[1]) < ((pAlarmArray[1])+1))
		{
			buzzer=1;
			led=1;
			delay(1);
			buzzer=0;
			led=0;
			delay(1);
		}
		else
		{
			buzzer=1;
			led=1;
			delay(2);
			buzzer=0;
			led=0;
			delay(2);
		}
	}
}
/*
void display_alarm(int Alarm_Num)
{
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String("Alarm");
	Lcd_Write_Char(Alarm_Num+48);

	Get_EEPROM_Alarm(Alarm_Num);

	Lcd_Set_Cursor(2,0);
	Lcd_Write_Char(pAlarmArray[2]/10+48);
	Lcd_Write_Char(pAlarmArray[2]%10+48);
	Lcd_Write_Char(':');
	Lcd_Write_Char(pAlarmArray[1]/10+48);
	Lcd_Write_Char(pAlarmArray[1]%10+48);
	Lcd_Write_Char(' ');
	Temp=(pAlarmArray[3]%10);
	Lcd_Set_Cursor(2,6);
	switch(pAlarmArray[3])
	{
		case 1:	Lcd_Write_String("Everyday  ");	break;
		case 2:	Lcd_Write_String("Every2days");	break;
		case 3:	Lcd_Write_String("Every3days");	break;
		case 4:	Lcd_Write_String("Every4days");	break;
		case 5:	Lcd_Write_String("Every5days");	break;
		case 6:	Lcd_Write_String("Every6days");	break;
		case 7:	Lcd_Write_String("Every7days");	break;

		default: Lcd_Write_String("???");	break;
	}
	Lcd_Set_Cursor(2,7);
	Lcd_Write_String("B1-");
	Lcd_Write_Char(pAlarmArray[4]/10+48);
	Lcd_Write_Char(pAlarmArray[4]%10+48);
	Lcd_Write_String(" B2-");
	Lcd_Write_Char(pAlarmArray[4]/10+48);
	Lcd_Write_Char(pAlarmArray[4]%10+48);

}
*/
