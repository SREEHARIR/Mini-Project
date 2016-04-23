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
void motor(int m1, int m2)
{
	for(i=0;i<m1;i++)
	{
		for(j=0;j<4;j++)
		{
			BOX=0x01;
			delay(5);
			BOX=0x04;
			delay(5);
			BOX=0x02;
			delay(5);
			BOX=0x08;
			delay(5);
		}
	}
	BOX=0x00;
	for(i=0;i<m2;i++)
	{
		for(j=0;j<4;j++)
		{
			BOX=0x10;
			delay(5);
			BOX=0x40;
			delay(5);
			BOX=0x20;
			delay(5);
			BOX=0x80;
			delay(5);
		}
	}
	BOX=0x00;
	//alarm();
}


void Refresh_New_Day(void)
{
	if(Prev_Day!=pRTCArrayTime[3])
	{
		alarms=Read_Byte_From_EEPROM(0x00);
		for(i=1;i<=alarms;i++)
		{
			Write_Byte_To_EEPROM((Alarm_Memory+1+((i-1)*6)),0);
		}
	}
	Prev_Day=pRTCArrayTime[3];
}
	


void Lcd_Port(char a)
{
	if(a & 1)
		D0 = 1;
	else
		D0 = 0;

	if(a & 2)
		D1 = 1;
	else
		D1 = 0;

	if(a & 4)
		D2 = 1;
	else
		D2 = 0;

	if(a & 8)
		D3 = 1;
	else
		D3 = 0;

	if(a & 16)
		D4 = 1;
	else
		D4 = 0;

	if(a & 32)
		D5 = 1;
	else
		D5 = 0;

	if(a & 64)
		D6 = 1;
	else
		D6 = 0;

	if(a & 128)
		D7 = 1;
	else
		D7 = 0;
}
void Lcd_Cmd(char a)
{
	RS = 0;						 // => RS = 0
	Lcd_Port(a);						 //Data transfer
	EN	= 1;						 // => E = 1
	delay(5);
	EN	= 0;						 // => E = 0
}

void Lcd_Clear()
{
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b)
{
	if(a == 1)
		Lcd_Cmd(0x80 + b);
	else if(a == 2)
		Lcd_Cmd(0xC0 + b);
}

void Lcd_Init()
{
	Lcd_Port(0x00);
	RS = 0;
	delay(5);
	///////////// Reset process from datasheet /////////
	Lcd_Cmd(0x30);
	delay(5);
	Lcd_Cmd(0x30);
	delay(5);
	Lcd_Cmd(0x30);
	/////////////////////////////////////////////////////
	Lcd_Cmd(0x38);		//function set
	Lcd_Cmd(0x0C);		//display on,cursor off,blink off
	Lcd_Cmd(0x01);		//clear display
	Lcd_Cmd(0x06);		//entry mode, set increment
}

void Lcd_Write_Char(char a)
{
	 RS = 1;						 // => RS = 1
	 Lcd_Port(a);						 //Data transfer
	 EN	= 1;						 // => E = 1
	 delay(1);
	 EN	= 0;						 // => E = 04
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	 Lcd_Write_Char(a[i]);
}
void DisplayTimeToLCD( unsigned char* pTimeArray )	 // Displays time in HH:MM:SS AM/PM format
{
	//pTimeArray=pRTCArrayTime;
	Lcd_Set_Cursor(2,0);
	// Display Hour
	Lcd_Write_Char( (pTimeArray[2]/10)+0x30 );
	Lcd_Write_Char( (pTimeArray[2]%10)+0x30 );

	//Display ':'
	Lcd_Write_Char(':');

	//Display Minutes
	Lcd_Write_Char( (pTimeArray[1]/10)+0x30 );
	Lcd_Write_Char( (pTimeArray[1]%10)+0x30 );

	//Display ':'
	Lcd_Write_Char(':');

	//Display Seconds
	Lcd_Write_Char( (pTimeArray[0]/10)+0x30 );
	Lcd_Write_Char( (pTimeArray[0]%10)+0x30 );

	//Display Space
	Lcd_Write_Char(' ');

	// Display mode
	/*
	switch(pTimeArray[3])
	{
	case AM_Time:	Lcd_Write_String("AM");	break;
	case PM_Time:	Lcd_Write_String("PM");	break;

	default: Lcd_Write_Char('H');	break;
	}
	*/
	Temp = (unsigned char)pTimeArray[4]+48;
	switch(pTimeArray[4])
	{
	case Sunday:	Lcd_Write_String("SUN");	break;
	case Monday:	Lcd_Write_String("MON");	break;
	case Tuesday:	Lcd_Write_String("TUE");	break;
	case Wednesday:	Lcd_Write_String("WED");	break;
	case Thursday:	Lcd_Write_String("THU");	break;
	case Friday:	Lcd_Write_String("FRI");	break;
	case Saturday:	Lcd_Write_String("SAT");	break;

	/*default: 	{Lcd_Write_String("?");
						Lcd_Write_Char(pTimeArray[4]+48);	break;}*/
	}
}

void DisplayDateToLCD(unsigned char* pDateArray )	 // Displays Date in DD:MM:YY @ Day format
{
	Lcd_Set_Cursor(2,8);

	// Display Date
	Lcd_Write_Char( (pDateArray[1]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[1]%10)+0x30 );

	//Display '/'
	Lcd_Write_Char('/');

	//Display Month
	Lcd_Write_Char( (pDateArray[2]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[2]%10)+0x30 );

	//Display '/'
	Lcd_Write_Char('/');

	//Display Year
	Lcd_Write_Char( (pDateArray[3]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[3]%10)+0x30 );

	//Display Space
	Lcd_Write_Char(' ');


	// Display Day

}


void DisplayAlarmToLCD( unsigned char* pAlarmArray )	 // Displays time in HH:MM:SS AM/PM format
{
	Lcd_Set_Cursor(1,0);
	// Display Hour
	Lcd_Write_Char( (pAlarmArray[2]/10)+0x30 );
	Lcd_Write_Char( (pAlarmArray[2]%10)+0x30 );

	//Display ':'
	Lcd_Write_Char(':');

	//Display Minutes
	Lcd_Write_Char( (pAlarmArray[1]/10)+0x30 );
	Lcd_Write_Char( (pAlarmArray[1]%10)+0x30 );

	//Display ':'
	//Lcd_Write_Char(':');

	//Display Seconds
//	Lcd_Write_Char( (pAlarmArray[3]/10)+0x30 );
//	Lcd_Write_Char( (pAlarmArray[3]%10)+0x30 );

	//Display Space
	Lcd_Write_Char(' ');

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
	
	Lcd_Set_Cursor(2,0);

	Lcd_Write_String(" B1-");

	Lcd_Write_Char( (pAlarmArray[4]/10)+0x30 );
	Lcd_Write_Char( (pAlarmArray[4]%10)+0x30 );

	Lcd_Write_String(" B2-");

	Lcd_Write_Char( (pAlarmArray[5]/10)+0x30 );
	Lcd_Write_Char( (pAlarmArray[5]%10)+0x30 );

	// Display mode
	/*
	switch(pTimeArray[3])
	{
	case AM_Time:	Lcd_Write_Char("AM");	break;
	case PM_Time:	Lcd_Write_Char("PM");	break;

	default: Lcd_Write_Char('H');	break;
	}
	*/
}
void __delay_us(unsigned int d)
{
	 unsigned int i, limit;
	 limit = d/15;

	 for(i=0;i<limit;i++);
}

void InitI2C(void)
{
	// Function Purpose: Set initial values of SCK and SDA pins
	// Make SDA and SCK pins input initially
	SDA = 1;
	SCK = 1;
}

void I2C_Start(void)
{
	// Function Purpose: I2C_Start sends start bit sequence
	Set_SCK_High;				// Make SCK pin high
	Set_SDA_High;				// Make SDA pin High
	__delay_us(HalfBitDelay);	// Half bit delay
	Set_SDA_Low;				// Make SDA Low
	__delay_us(HalfBitDelay);	// Half bit delay
}

void I2C_ReStart(void)
{
	// Function Purpose: I2C_ReStart sends start bit sequence
	Set_SCK_Low;				// Make SCK pin low

	__delay_us(HalfBitDelay/2);	// Data pin should change it's value,
								// when it is confirm that SCK is low
	Set_SDA_High;				// Make SDA pin High

	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SCK_High;				// Make SCK pin high
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SDA_Low;				// Make SDA Low
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
}

void I2C_Stop(void)
{
	//Function : I2C_Stop sends stop bit sequence
	Set_SCK_Low;				// Make SCK pin low

	__delay_us(HalfBitDelay/2);	// Data pin should change it's value,
								// when it is confirm that SCK is low
	Set_SDA_Low;				// Make SDA pin low

	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SCK_High;				// Make SCK pin high
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SDA_High;				// Make SDA high
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
}

void I2C_Send_ACK(void)
{
	//Function : I2C_Send_ACK sends ACK bit sequence
	Set_SCK_Low;				// Make SCK pin low
	__delay_us(HalfBitDelay/2);	// Data pin should change it's value,
								// when it is confirm that SCK is low
	Set_SDA_Low;				// Make SDA Low
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SCK_High;				// Make SCK pin high
	__delay_us(HalfBitDelay);	// Half bit delay
}

void I2C_Send_NACK(void)
{
	//Function : I2C_Send_NACK sends NACK bit sequence
	Set_SCK_Low;				// Make SCK pin low
	__delay_us(HalfBitDelay/2);	// Data pin should change it's value,
								// when it is confirm that SCK is low
	Set_SDA_High;				// Make SDA high
	__delay_us(HalfBitDelay/2);	// 1/4 bit delay
	Set_SCK_High;				// Make SCK pin high
	__delay_us(HalfBitDelay);	// Half bit delay
}

bit I2C_Write_Byte(unsigned char Byte)
{
	// Function Purpose: I2C_Write_Byte transfers one byte
	unsigned char i;		// Variable to be used in for loop

	for(i=0;i<8;i++)		// Repeat for every bit
	{
		Set_SCK_Low;		// Make SCK pin low

		__delay_us(HalfBitDelay/2);	// Data pin should change it's value,
									// when it is confirm that SCK is low

		if((Byte<<i)&0x80)	// Place data bit value on SDA pin
			Set_SDA_High;	// If bit is high, make SDA high
		else				// Data is transferred MSB first
			Set_SDA_Low;	// If bit is low, make SDA low

		__delay_us(HalfBitDelay/2);	// Toggle SCK pin
		Set_SCK_High;				// So that slave can
		__delay_us(HalfBitDelay);	// latch data bit
		}

	// Get ACK from slave
	Set_SCK_Low;
		Set_SDA_High;
		__delay_us(HalfBitDelay);
		Set_SCK_High;
		__delay_us(HalfBitDelay);

	return SDA;
}

unsigned char I2C_Read_Byte(void)
{
	// Function Purpose: I2C_Read_Byte reads one byte
	unsigned char i, d, RxData = 0;

	for(i=0;i<8;i++)
	{
		Set_SCK_Low;					// Make SCK pin low
		Set_SDA_High;					// Don't drive SDA
		__delay_us(HalfBitDelay);		// Half bit delay
		Set_SCK_High;					// Make SCK pin high
		__delay_us(HalfBitDelay/2);		// 1/4 bit delay
		d = SDA;							// Capture Received Bit
		RxData = RxData|(d<<(7-i));	 	// Copy it in RxData
		__delay_us(HalfBitDelay/2);		// 1/4 bit delay
	}

		return RxData;						// Return received byte
}

void Write_Byte_To_RTC(unsigned char Address, unsigned char DataByte)
{
	/*
	// Function Purpose: Write_Byte_To_RTC writes a single byte on given address
	// Address can have any value fromm 0 to 0xFF, and DataByte can have a value of 0 to 0xFF.
	*/
	I2C_Start();										// Start i2c communication

	// Send i2c address of DS1307 with write command
	while(I2C_Write_Byte(Device_Address_RTC + 0) == 1)// Wait until device is free
	{	I2C_Start();	}

	I2C_Write_Byte(Address);							// Write Address byte
	I2C_Write_Byte(DataByte);							// Write data byte
	I2C_Stop();											// Stop i2c communication
}

unsigned char Read_Byte_From_RTC(unsigned char Address)
{
	/*
	Function Purpose: Read_Byte_From_RTC reads a single byte from given address
	Address can have any value fromm 0 to 0xFF.
	*/
	unsigned char Byte = 0;								// Variable to store Received byte

	I2C_Start();										// Start i2c communication

	// Send i2c address of DS1307 with write command
	while(I2C_Write_Byte(Device_Address_RTC + 0) == 1)// Wait until device is free
	{	I2C_Start();	}

	I2C_Write_Byte(Address);							// Write Address byte
	I2C_ReStart();										// Restart i2c

	// Send i2c address of DS1307 RTC with read command
	I2C_Write_Byte(Device_Address_RTC + 1);

	Byte = I2C_Read_Byte();								// Read byte from EEPROM

	// Make SCK low, so that slave can stop driving SDA pin
	// Send a NACK to indiacate single byte read is complete
	I2C_Send_NACK();

	// Send start bit and then stop bit to stop transmission
	Set_SDA_Low;				// Make SDA Low
	__delay_us(HalfBitDelay);	// Half bit delay
	Set_SDA_High;				// Make SDA high
	__delay_us(HalfBitDelay);	// Half bit delay

	return Byte;				// Return the byte received from 24LC64 EEPROM
}

void Write_Bytes_To_RTC(unsigned char Address,unsigned char* pData,unsigned char NoOfBytes)
{
	/*
	Function Purpose: Write_Bytes_To_RTC writes mulitple bytes from given starting address.
	Address can have any value fromm 0 to 0xFF and pData is pointer to the array
	containing NoOfBytes bytes in it. NoOfBytes is the number of bytes to write.
	*/
	unsigned int i;

	I2C_Start();										// Start i2c communication

	// Send i2c address of DS1307 with write command
	while(I2C_Write_Byte(Device_Address_RTC + 0) == 1)// Wait until device is free
	{	I2C_Start();	}

	I2C_Write_Byte(Address);							// Write Address byte

	for(i=0;i<NoOfBytes;i++)							// Write NoOfBytes
		I2C_Write_Byte(pData[i]);						// Write data byte

	I2C_Stop();											// Stop i2c communication
}

void Read_Bytes_From_RTC(unsigned char Address, unsigned char* pData, unsigned int NoOfBytes)
{
	/*
 Function Purpose: Read_Bytes_From_RTC reads a NoOfBytes bytes from given starting address.
 Address can have any value fromm 0 to 0xFF. NoOfBytes is the number of bytes to write.
 Read bytes are returned in pData array.
	*/
	unsigned int i;

	I2C_Start();										// Start i2c communication

	// Send i2c address of DS1307 with write command
	while(I2C_Write_Byte(Device_Address_RTC + 0) == 1)// Wait until device is free
	{	I2C_Start();	}

	I2C_Write_Byte(Address);							// Write Address byte
	I2C_ReStart();										// Restart i2c

	// Send i2c address of DS1307 RTC with read command
	I2C_Write_Byte(Device_Address_RTC + 1);

	pData[0] = I2C_Read_Byte();							// Read First byte from EEPROM

	for(i=1;i<NoOfBytes;i++)							// Read NoOfBytes
	{
		I2C_Send_ACK();					// Give Ack to slave to start receiving next byte
		pData[i] = I2C_Read_Byte();		// Read next byte from EEPROM
	}

	// Make SCK low, so that slave can stop driving SDA pin
	// Send a NACK to indiacate read operation is complete
	I2C_Send_NACK();

	// Send start bit and then stop bit to stop transmission
	Set_SDA_Low;				// Make SDA Low
	__delay_us(HalfBitDelay);	// Half bit delay
	Set_SDA_High;				// Make SDA high
	__delay_us(HalfBitDelay);	// Half bit delay
}

bit isRTCStopped()
{
  //bit 7 of the seconds register stopps the clock when high
  return ((pRTCArrayTime[0] & 0x80) == 0x80);
}

void stopRTC()
{
  //"Bit 7 of register 0 is the clock halt (CH) bit.
  //When this bit is set to a 1, the oscillator is disabled."
  pRTCArrayTime[0] = pRTCArrayTime[0] | 0x80;
  Write_Byte_To_RTC(0x00, pRTCArrayTime[0]);
}

void startRTC()
{
  //"Bit 7 of register 0 is the clock halt (CH) bit.
  //When this bit is set to a 1, the oscillator is disabled."
  pRTCArrayTime[0] = pRTCArrayTime[0] & ~0x80;
  Write_Byte_To_RTC(0x00, pRTCArrayTime[0]);
}

void Set_RTC_Time(unsigned char Day, unsigned char Mode, unsigned char Hours, unsigned char Mins, unsigned char Secs)
{
	/*
	Function Purpose: Set_RTC_Time sets given time in RTC registers.
	Mode can have a value AM_Time or PM_Time	or TwentyFourHoursMode only.
	Hours can have value from 0 to 23 only.
	Mins can have value from 0 to 59 only.
	Secs can have value from 0 to 59 only.
	*/
	// Convert Hours, Mins, Secs into BCD
	pRTCArrayTime[0] = (((unsigned char)(Secs/10))<<4)|((unsigned char)(Secs%10));
	pRTCArrayTime[1] = (((unsigned char)(Mins/10))<<4)|((unsigned char)(Mins%10));
	pRTCArrayTime[2] = (((unsigned char)(Hours/10))<<4)|((unsigned char)(Hours%10));
	pRTCArrayTime[3] = (((unsigned char)(Day/10))<<4)|((unsigned char)(Day%10));

	//pRTCArrayTime[0] = pRTCArrayTime[0]&(0x00);

	switch(Mode)	// Set mode bits
	{
	case AM_Time: 	pRTCArrayTime[2] |= 0x40;	break;
	case PM_Time: 	pRTCArrayTime[2] |= 0x60;	break;

	default:	break;	// do nothing for 24HoursMode
	}

	// WritepRTCArray to DS1307
	Write_Bytes_To_RTC(0x00, pRTCArrayTime, 4);
}

void Get_RTC_Time(void)
{
	/*
 Function Purpose: Get_RTC_Time returns current time from RTC registers.
 Pointer to pRTCArray is returned, in this array
 pRTCArray[3] can have a value AM_Time or PM_Time	or TwentyFourHoursMode only.
 pRTCArray[2] (Hours byte) can have value from 0 to 23 only.
 pRTCArray[1] (Mins byte) can have value from 0 to 59 only.
 pRTCArray[0] (Secs byte) can have value from 0 to 59 only.
	*/
	// Read Hours, Mins, Secs register from RTC
	Read_Bytes_From_RTC(0x00, pRTCArrayTime, 4);

	// Convert Secs back from BCD into number
	Temp = pRTCArrayTime[0];
	pRTCArrayTime[0] = ((Temp&0x7F)>>4)*10 + (Temp&0x0F);

	// Convert Mins back from BCD into number
	Temp = pRTCArrayTime[1];
	pRTCArrayTime[1] = (Temp>>4)*10 + (Temp&0x0F);

	// Convert Days back from BCD into number
	pRTCArrayTime[4] = pRTCArrayTime[3];
	//pRTCArrayTime[4] = (Temp>>4)*10 + (Temp&0x0F);

	// Convert Hours back from BCD into number
	if(pRTCArrayTime[2]&0x40)	// if 12 hours mode
	{
		if(pRTCArrayTime[2]&0x20)	// if PM Time
			pRTCArrayTime[3] = PM_Time;
		else		// if AM time
			pRTCArrayTime[3] = AM_Time;

		Temp = pRTCArrayTime[2];
		pRTCArrayTime[2] = ((Temp&0x1F)>>4)*10 + (Temp&0x0F);
	}
	else		// if 24 hours mode
	{
		Temp = pRTCArrayTime[2];
		pRTCArrayTime[2] = (Temp>>4)*10 + (Temp&0x0F);
		pRTCArrayTime[3] = TwentyFourHoursMode;
	}

//	return pRTCArrayTime;
}

void Set_RTC_Date(unsigned char Date, unsigned char Month, unsigned char Year)
{
	/* Function Purpose: Set_RTC_Date sets given date in RTC registers.
	Year can have a value from 0 to 99 only.
	Month can have value from 1 to 12 only.
	Date can have value from 1 to 31 only.
	Day can have value from 1 to 7 only. Where 1 means Monday, 2 means Tuesday etc.
	*/
	// Convert Year, Month, Date, Day into BCD
	//pRTCArrayDate[0] = (((unsigned char)(Day/10))<<4)|((unsigned char)(Day%10));
	pRTCArrayDate[0] = (((unsigned char)(Date/10))<<4)|((unsigned char)(Date%10));
	pRTCArrayDate[1] = (((unsigned char)(Month/10))<<4)|((unsigned char)(Month%10));
	pRTCArrayDate[2] = (((unsigned char)(Year/10))<<4)|((unsigned char)(Year%10));

	// WritepRTCArray to DS1307
	Write_Bytes_To_RTC(0x04, pRTCArrayDate, 3);
}

void Get_RTC_Date(void)
{
	/*
	Function Purpose: Get_RTC_Date returns current date from RTC registers.
	Pointer to pRTCArray is returned, in this array
	pRTCArray[3] (Year byte) can have value from 0 to 99 only.
	pRTCArray[2] (Month byte) can have value from 1 to 12 only.
	pRTCArray[1] (Date byte) can have value from 1 to 31 only.
	pRTCArray[0] (Day byte) can have value from 1 to 7 only.
	*/
	// Read Hours, Mins, Secs register from RTC
	Read_Bytes_From_RTC(0x04, pRTCArrayDate, 3);

	// Convert Date back from BCD into number
	Temp = pRTCArrayDate[0];
	pRTCArrayDate[0] = (Temp>>4)*10 + (Temp&0x0F);

	// Convert Month back from BCD into number
	Temp = pRTCArrayDate[1];
	pRTCArrayDate[1] = (Temp>>4)*10 + (Temp&0x0F);

	// Convert Year back from BCD into number
	Temp = pRTCArrayDate[2];
	pRTCArrayDate[2] = (Temp>>4)*10 + (Temp&0x0F);

//	return pRTCArrayDate;
}
