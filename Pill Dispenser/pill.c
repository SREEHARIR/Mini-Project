#include<reg52.h>
#include<intrins.h>

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

sbit scl = P0^0;
sbit sda = P0^1;

sbit sw_1 = P1^2;
sbit sw_2 = P1^3;
sbit ir = P1^4;
sbit buzzer = P1^5;

unsigned char tempchr,mem=0xA0,rtc=0xD0,sec=0x00,min=0x01,hr=0x02,day=0x03;
int i=0,j=0,fn=0,fn2=0,dat=0,dat_bcd=0,alarms=0,tmpint=0; 
/*
fn -> 0-time, 1-set_time, 2-add_alarm(yes/no), 3-remove_alarm(yes/no) 
*/
/*
saving in eeprom -> 
0x00   - num of alarms
0x01*i - min of alarm i
0x02*i - hr of alarm i
0x03*i - day of alarm i
0x04*i - motor 1 rotation & motor 2 rotation
*/
char tim[4],timchk[4]; 
int timi[4];
/* 
tim 		-> 0-sec,		1-min, 2-hr, 3-day //read from rtc
timchk  -> 0-motor, 1-min, 2-hr, 3-day //read from mem
*/
void delay(int i);
void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear();
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init();
void Lcd_Write_Char(char a);
void Lcd_Write_String(char *a);
void start(void);
void stop();
void write(unsigned char dat);
void ack(void);
void noack(void);
unsigned char read();
void save_i2c(char dev,char addr,char ch);
char read_i2c(char dev,char addr);
void time(void);
void display_time(void);
void check_alarm(void);
unsigned char Bcd_to_dec (unsigned char val);
unsigned char Dec_to_bcd (unsigned char val);

	
// TODO //
//void interpt(void);
void funtions(void);
void motor(int m1,int m2);
void sort(void);
void alarm(void);
void display_alarm(int num);
void delete_alarm(int num);
void add_alarm(void);
void wipe(void);
// TODO //

void delay(int i)
{
	int j;
	while(i--)
		for(j=0;j<1000;j++);
}

unsigned char Dec_to_bcd (unsigned char val)
{
  return (( (val/10)<<4) + (val%10));
}

unsigned char Bcd_to_dec (unsigned char val)
{
  return (( (val>>4)*10) + (val%16));
}
/*
char lowToASCII(char b) 
{
  b = b & 0x0f;
  if(b < 10) {
    //0 is ASCII 48
    return 48+b;
  }
  //A is ASCII 55
  return 55+b;
}

char highToASCII(char b)
{
  return lowToASCII(b >> 4);
}

void time()
{
	tim[0]=read_i2c(rtc,sec);
	tim[1]=read_i2c(rtc,min);
	tim[2]=read_i2c(rtc,hr);
	tim[3]=read_i2c(rtc,day);
	
	//tim[0]=tim[0] & 0x7f;
	//tim[2]=tim[2] & 0x1f;
	
	//timi[0]=Bcd_to_dec(tim[0]);
	//timi[1]=Bcd_to_dec(tim[1]);
	//timi[2]=Bcd_to_dec(tim[2]);
	//timi[3]=Bcd_to_dec(tim[3]);
	
	
}
*/
void display_time()
{
	Lcd_Set_Cursor(2,0);
	Lcd_Write_Char((timi[2] & 0x3f));
	Lcd_Write_Char((timi[2]));
	Lcd_Write_Char(':');
	Lcd_Write_Char((timi[1] & 0x7f));
	Lcd_Write_Char((timi[1]));
	Lcd_Write_Char(':');
	Lcd_Write_Char((timi[0] & 0x7f));
	Lcd_Write_Char((timi[0]));
	Lcd_Write_Char(' ');
	
	if((timi[0] & 0x80) == 0x80)
	Lcd_Write_Char('1');
	
	else
	Lcd_Write_Char('0');
	
	switch(timi[3])
	{
		case 1:
			Lcd_Write_String("SUN");
			break;
		case 2:
			Lcd_Write_String("MON");
			break;
		case 3:
			Lcd_Write_String("TUE");
			break;
		case 4:
			Lcd_Write_String("WED");
			break;
		case 5:
			Lcd_Write_String("THU");
			break;
		case 6:
			Lcd_Write_String("FRI");
			break;
		case 7:
			Lcd_Write_String("SAT");
			break;
		default:
			Lcd_Write_String("ERR");
			break;
	}
}

void check_alarm(void)
{
	alarms=read_i2c(mem,0x00);
	for(i=0;i<alarms;i++)
	{
		for(j=0;j<4;j++)
		{
			timchk[j]=read_i2c(mem,((i*4)+j+1));
		}
		tempchr=((timchk[2]&0xF0)>>4);
		timchk[2]=timchk[2]&0x0F;
		if((timchk[1] == tim[1]) && (timchk[2] == tim[2]) && (timchk[3] == tim[3])) //check min hr day
		{
			timchk[3]+=tempchr;
			tempchr=(tempchr<<4);
			timchk[3]=(timchk[3]|tempchr);
			save_i2c(mem,(((i*4)+(2+1))),timchk[3]);       // save if alarm over in day register bits 7654
			motor(((timchk[0]&0xF0)>>4),(timchk[0]&0x0F));
			alarm();
		}
	}
}

void alarm()
{
	while(1)
	{
		buzzer=1;
		if((sw_1==1) || (sw_2==1) || (ir==0))
		{
			buzzer=0;
			while((sw_1==1) && (sw_2==1));
			return;
		}
		tempchr=read_i2c(rtc,min);
		if((tempchr%16) > ((tim[1]%16)+1))
		{
			buzzer=1;
			delay(1);
			buzzer=0;
			delay(1);
		}
		else if((tempchr/16) > ((tim[1]/16)+1))
		{
			buzzer=1;
			delay(1);
			buzzer=0;
			delay(1);
		}
	}
}
void display_alarm(int num)
{
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String("Alrm");
	Lcd_Write_Char(num+48);
	for(i=0;i<4;i++)
	{
		timchk[i]=read_i2c(mem,((num*4)+i+1));
	}
	Lcd_Set_Cursor(2,0);
	Lcd_Write_Char(timchk[2]/16+48);
	Lcd_Write_Char(timchk[2]%16+48);
	Lcd_Write_Char(':');
	Lcd_Write_Char(timchk[1]/16+48);
	Lcd_Write_Char(timchk[1]%16+48);
	Lcd_Write_Char(':');
	Lcd_Write_Char(' ');
	tempchr=(timchk[3]>>4);
	Lcd_Set_Cursor(2,6);
	if(tempchr==1)
	{
		Lcd_Write_String("Everyday  ");
	}
	if(tempchr==2)
	{
		Lcd_Write_String("Every2days");
	}
	if(tempchr==3)
	{
		Lcd_Write_String("Every3days");
	}
	if(tempchr==4)
	{
		Lcd_Write_String("Every4days");
	}
	if(tempchr==7)
	{
		Lcd_Write_String("Every7days");
	}
	tempchr=(timchk[0]&0xF0);
	tempchr=(timchk[0]>>4);
	Lcd_Set_Cursor(2,7);
	Lcd_Write_String("B1-");
	Lcd_Write_Char(tempchr);
	tempchr=(timchk[0]&0x0F);
	Lcd_Write_String("B2-");
	Lcd_Write_Char(tempchr);
}

void delete_alarm(int num)
{
	for(i=0;i<alarms;i++)
	{
		for(j=0;j<4;j++)
		{
			timchk[j]=read_i2c(mem,(((num+i+1)*4)+j+1));
		}
		for(j=0;j<4;j++)
		{
			save_i2c(mem,(((num+i)*4)+j+1),timchk[i]);
		}
	}
}

void motor(int m1, int m2)
{
	
	for(i=0;i<m1;i++)
	{
		for(j=0;j<4;j++)
		{
			BOX=0x01;
			delay(3);
      BOX=0x04;
			delay(3);
      BOX=0x02;
			delay(3);
      BOX=0x08;
			delay(3);
		}
	}
	BOX=0x00;
	for(i=0;i<m2;i++)
	{
		for(j=0;j<4;j++)
		{
			BOX=0x10;
			delay(3);
      BOX=0x40;
			delay(3);
      BOX=0x20;
			delay(3);
      BOX=0x80;
			delay(3);
		}
	}
	BOX=0x00;
	
}
void functions()
{
	int change=0;
	if(fn==0)
	{
		return;
	}
	time();
	if(fn==1)
	{
		Lcd_Clear();
		Lcd_Set_Cursor(1,0);
		Lcd_Write_String("SET TIME");
		display_time();
		fn2=0;
		while(1)
		{
			if(sw_1==1)
			{
				while(sw_1==1);
				fn++;
				break;
			}
			if(sw_2==1)
			{
				while(sw_2==1);
				if(fn2==0)
				{
					Lcd_Clear();
					Lcd_Set_Cursor(1,0);
					Lcd_Write_String("SET TIME - Hour ");
					//display_time();
					dat=0;
					change=0;
					while(1)
					{
						if(sw_1==1)
						{
							while(sw_1==1);
							fn2++;
							if(change!=0)
							{
								dat_bcd=((dat/10)<<4)+((dat%10));
								save_i2c(rtc,hr,dat);
								time();
								change=0;
							}
							break;
						}
						if(sw_2==1)
						{
							change=1;
							while(sw_2==1);
							dat++;
							if(dat>=24)
							{
								dat=0;
							}
							Lcd_Set_Cursor(2,0);
							Lcd_Write_Char((dat/10)+48);
							Lcd_Write_Char((dat%10)+48);
						}
					}
				}
				if(fn2==1)
				{
					//Lcd_Clear();
					Lcd_Set_Cursor(1,0);
					Lcd_Write_String("SET TIME-Minute");
					//display_time();
					dat=0;
					change=0;
					while(1)
					{
						if(sw_1==1)
						{
							while(sw_1==1);
							fn2++;
							if(change!=0)
							{
								dat_bcd=((dat/10)<<4)+((dat%10));
								save_i2c(rtc,min,dat);
								time();
								change=0;
							}
							break;
						}
						if(sw_2==1)
						{
							change=1;
							while(sw_2==1);
							dat++;
							if(dat>=60)
							{
								dat=0;
							}
							Lcd_Set_Cursor(2,3);
							Lcd_Write_Char((dat/10)+48);
							Lcd_Write_Char((dat%10)+48);
						}
					}
				}
				if(fn2==2)
				{
					Lcd_Clear();
					Lcd_Set_Cursor(1,0);
					Lcd_Write_String("    SET DAY     ");
					//display_time();	
					Lcd_Set_Cursor(2,9);
					Lcd_Write_String("SUN");
					dat=1;
					change=0;
					while(1)
					{
						if(sw_1==1)
						{
							while(sw_1==1);
							fn2++;
							if(change!=0)
							{
								dat_bcd=((dat/10)<<4)+((dat%10));
								save_i2c(rtc,day,dat);
								time();
								change=0;
							}
							break;
						}
						if(sw_2==1)
						{
							change=1;
							while(sw_2==1);
							dat++;
							if(dat>=7)
							{
								dat=1;
							}
							Lcd_Set_Cursor(2,9);
							switch(dat)
							{
								case 1:
									Lcd_Write_String("SUN");
									break;
								case 2:
									Lcd_Write_String("MON");
									break;
								case 3:
									Lcd_Write_String("TUE");
									break;
								case 4:
									Lcd_Write_String("WED");
									break;
								case 5:
									Lcd_Write_String("THU");
									break;
								case 6:
									Lcd_Write_String("FRI");
									break;
								case 7:
									Lcd_Write_String("SAT");
									break;
								default:
									Lcd_Write_String("ERR");
									break;
							}
						}
					}
				}
			}
		}
	}
	if(fn==2)
	{
		Lcd_Clear();
		Lcd_Set_Cursor(1,0);
		Lcd_Write_String("   ADD ALARMS   ");
		display_time();
		dat=0;
		change=0;
		while(1)
		{
			if(sw_1==1)
			{
				while(sw_1==1);
				fn++;
				if(change!=0)
				{
					alarms++;
					for(i=0;i<4;i++)
					{
						save_i2c(mem,((alarms*4)+i+1),timchk[i]);
					}
				}
				break;
			}
			if(sw_2==1)
			{
				while(sw_2==1);
				change=1;
				dat++;
				if(sw_1==1)
				{
					while(sw_1==1);
					i++;
					break;
				}
				if(sw_2==1)
				{
					fn2=0;
					while(sw_2==1);
					if(fn2==0)
					{
						Lcd_Set_Cursor(2,12);
						Lcd_Write_String("Hour");
						Lcd_Set_Cursor(2,0);
						Lcd_Write_String("00:00:00   ");
						dat=0;
						change=0;
						while(1)
						{
							if(sw_1==1)
							{
								while(sw_1==1);
								fn2++;
								if(change!=0)
								{
									dat_bcd=((dat/10)<<4)+((dat%10));
									timchk[2]=dat_bcd;
									//save_i2c(rtc,hr,dat);
									change=0;
								}
								break;
							}
							if(sw_2==1)
							{
								change=1;
								while(sw_2==1);
								dat++;
								if(dat>=24)
								{
									dat=0;
								}
								Lcd_Set_Cursor(2,0);
								Lcd_Write_Char((dat/10)+48);
								Lcd_Write_Char((dat%10)+48);
							}
						}
					}
					if(fn2==1)
					{
						Lcd_Set_Cursor(2,12);
						Lcd_Write_String("Min ");
						Lcd_Set_Cursor(2,3);
						Lcd_Write_String("00:00   ");
						dat=0;
						change=0;
						while(1)
						{
							if(sw_1==1)
							{
								while(sw_1==1);
								fn2++;
								if(change!=0)
								{
									dat_bcd=((dat/10)<<4)+((dat%10));
									timchk[1]=dat_bcd;
									//save_i2c(rtc,min,dat);
									change=0;
								}
								break;
							}
							if(sw_2==1)
							{
								change=1;
								while(sw_2==1);
								dat++;
								if(dat>=60)
								{
									dat=0;
								}
								Lcd_Set_Cursor(2,3);
								Lcd_Write_Char((dat/10)+48);
								Lcd_Write_Char((dat%10)+48);
							}
						}
					}
					if(fn2==2)
					{
						Lcd_Set_Cursor(2,12);
						Lcd_Write_String("Day ");
						Lcd_Set_Cursor(2,0);
						Lcd_Write_String("Everyday");
						dat=0;
						change=0;
						while(1)
						{
							if(sw_1==1)
							{
								while(sw_1==1);
								fn2++;
								if(change!=0)
								{
									dat_bcd=((dat/10)<<4)+((dat%10));
									timchk[3]=dat_bcd;
									//save_i2c(rtc,day,dat);
									change=0;
								}
								break;
							}
							if(sw_2==1)
							{
								change=1;
								while(sw_2==1);
								dat++;
								if(dat>=7)
								{
									dat=1;
								}
								Lcd_Set_Cursor(2,10);	
								switch(dat)
								{
									case 1:	
										Lcd_Write_String("Everyday        ");
										break;
									case 2:
										Lcd_Set_Cursor(2,0);	
										Lcd_Write_String("SUN ");
										Lcd_Write_String("TUE ");
										Lcd_Write_String("THU ");
										Lcd_Write_String("SAT ");
										break;
									case 3:
										Lcd_Set_Cursor(2,0);	
										Lcd_Write_String("SUN ");
										Lcd_Write_String("WED ");
										Lcd_Write_String("SAT ");
										Lcd_Write_String("    ");
										break;
									case 4:
										Lcd_Set_Cursor(2,0);	
										Lcd_Write_String("SUN ");
										Lcd_Write_String("THU ");
										Lcd_Write_String("        ");
										break;
									case 5:
									case 6:
									case 7:
										dat=7;
										Lcd_Write_String("SUN ");
										Lcd_Write_String("            ");
										break;
									default:
										Lcd_Write_String("ERR             ");
										break;
								}
							}
						}
					}
					if(fn2==3)
					{
						Lcd_Set_Cursor(2,5);
						Lcd_Write_String("Box 1 Turns ");
						Lcd_Set_Cursor(2,0);
						Lcd_Write_String("00  ");
						dat=0;
						change=0;
						while(1)
						{
							if(sw_1==1)
							{
								while(sw_1==1);
								fn2++;
								if(change!=0)
								{
									dat_bcd=((dat/10)<<4)+((dat%10));
									timchk[0]=(dat_bcd<<4);
									//save_i2c(rtc,min,dat);
									change=0;
								}
								break;
							}
							if(sw_2==1)
							{
								change=1;
								while(sw_2==1);
								dat++;
								if(dat>=11)
								{
									dat=0;
								}
								Lcd_Set_Cursor(2,0);
								Lcd_Write_Char((dat/10)+48);
								Lcd_Write_Char((dat%10)+48);
							}
						}
					}
					
					if(fn2==4)
					{
						Lcd_Set_Cursor(2,5);
						Lcd_Write_String("Box 2 Turns ");
						Lcd_Set_Cursor(2,0);
						Lcd_Write_String("00  ");
						dat=0;
						change=0;
						while(1)
						{
							if(sw_1==1)
							{
								while(sw_1==1);
								fn2++;
								if(change!=0)
								{
									dat_bcd=((dat/10)<<4)+((dat%10));
									timchk[0]+=(dat_bcd);
									//save_i2c(rtc,min,dat);
									change=0;
								}
								break;
							}
							if(sw_2==1)
							{
								change=1;
								while(sw_2==1);
								dat++;
								if(dat>=11)
								{
									dat=0;
								}
								Lcd_Set_Cursor(2,0);
								Lcd_Write_Char((dat/10)+48);
								Lcd_Write_Char((dat%10)+48);
							}
						}
					}
				}
			}
		}
	}
	if(fn==3)
	{
		Lcd_Clear();
		Lcd_Set_Cursor(1,0);
		Lcd_Write_String(" REMOVE ALARMS  ");
		//display_time();
		dat=0;
		while(1)
		{
			if(sw_1==1)
			{
				while(sw_1==1);
				fn++;
				break;
			}
			if(sw_2==1)
			{
				while(sw_2==1);
				dat++;
				if(alarms==0)
				{
					Lcd_Set_Cursor(2,0);
					Lcd_Write_String("No Alarms SET   ");
					delay(20);
					break;
				}
				for(i=0;i<alarms;i++)
				{
					display_alarm(i);
					while(1)
					{
						if(sw_1==1)
						{
							while(sw_1==1);
							break;
						}
						if((sw_2)==1)
						{
							while(sw_2==1);
							alarms--;
							delete_alarm(i);
							break;
						}
					}
				}
			}
		}
	}
	//sort();
	if(fn>=4)
	{
		fn=0;
	}
}

void main()
{		
		
	Lcd_Init();
	Lcd_Clear();
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String("Pill Dispenser 3.0");
	delay(100);
	fn=0;
	alarms=0;
	
	//save_i2c(mem,0X00,0X00);   // comment this line after first execuiton and upload again 
	
	save_i2c(rtc,sec,0x80);
	
	save_i2c(rtc,min,0x01);
	save_i2c(rtc,hr,0x01 | 0x40);
	save_i2c(rtc,day,0x01);
	save_i2c(rtc,sec,(0x01)&(~0x80));
	
	while(1)
	{
		time();
		display_time();
		//check_alarm();
		if(sw_1 == 1)
		{
			while(sw_1==1);
			fn++;
			Lcd_Set_Cursor(1,15);
			Lcd_Write_Char(fn+48);
			delay(100);
			functions();			
		}
	}
	
	/*
	save_i2c(rtc,sec,0X00);
	save_i2c(rtc,min,0X25);
	save_i2c(rtc,hr,0X08);
	
	Lcd_Clear();
	Lcd_Set_Cursor(1,0);
		
	for(i=0;i<16;i++)
	{
	  //save_i2c(mem,i,i+'a');
		ch=read_i2c(mem,i);
		Lcd_Write_Char(ch) ;
	}
	
	while(1)
	{
		Lcd_Set_Cursor(2,0);
		ch=read_i2c(rtc,hr);
		Lcd_Write_Char(ch/16+48);
		Lcd_Write_Char(ch%16+48);
		Lcd_Write_Char('-');
		ch=read_i2c(rtc,min);
		Lcd_Write_Char(ch/16+48);
		Lcd_Write_Char(ch%16+48);
		Lcd_Write_Char('-');
		ch=read_i2c(rtc,sec);
		Lcd_Write_Char(ch/16+48);
		Lcd_Write_Char(ch%16+48);
	}
	*/
	
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
  RS = 0;             // => RS = 0
  Lcd_Port(a);             //Data transfer
  EN  = 1;             // => E = 1
  delay(5);
  EN  = 0;             // => E = 0
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
  Lcd_Cmd(0x38);    //function set
  Lcd_Cmd(0x0C);    //display on,cursor off,blink off
  Lcd_Cmd(0x01);    //clear display
  Lcd_Cmd(0x06);    //entry mode, set increment
}

void Lcd_Write_Char(char a)
{
   RS = 1;             // => RS = 1
   Lcd_Port(a);             //Data transfer
   EN  = 1;             // => E = 1
   delay(1);
   EN  = 0;             // => E = 04
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	 Lcd_Write_Char(a[i]);
}
/*
void DisplayTimeToLCD( unsigned char* pTimeArray )   // Displays time in HH:MM:SS AM/PM format
{
	Lcd_Clear();      // Move cursor to zero location and clear screen
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
	switch(pTimeArray[3])
	{
	case AM_Time:	Lcd_Write_Char("AM");	break;
	case PM_Time:	Lcd_Write_Char("PM");	break;

	default: Lcd_Write_Char('H');	break;
	}
}

void DisplayDateOnLCD( unsigned char* pDateArray )   // Displays Date in DD:MM:YY @ Day format
{
	Lcd_Set_Cursor(2,11);
	
	// Display Date
	Lcd_Write_Char( (pDateArray[1]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[1]%10)+0x30 );

	//Display '/'
	Lcd_Write_Char(':');

	//Display Month
	Lcd_Write_Char( (pDateArray[2]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[2]%10)+0x30 );

	//Display '/'
	WriteDataToLCD(':');

	//Display Year
	Lcd_Write_Char( (pDateArray[3]/10)+0x30 );
	Lcd_Write_Char( (pDateArray[3]%10)+0x30 );

	//Display Space
	Lcd_Write_Char(' ');

	// Display Day
	switch(pDateArray[0])
	{
	case Monday:	Lcd_Write_String("MON");	break;
	case Tuesday:	Lcd_Write_String("TUE");	break;
	case Wednesday:	Lcd_Write_String("WED");	break;
	case Thursday:	Lcd_Write_String("THU");	break;
	case Friday:	Lcd_Write_String("FRI");	break;
	case Saturday:	Lcd_Write_String("SAT");	break;
	case Sunday:	Lcd_Write_String("SUN");	break;

	default: Lcd_Write_String("???");	break;
	}
}
*/
void start(void)
{
	/*to start make
	high to low of sda with a clock is high
	*/

	sda=1;
	scl=1;
	_nop_();
	_nop_();	
	sda=0;
	scl=0;
}

void stop()
{
	/* to stop the i2c
	make an low to high of sda with scl =1*/

	sda = 0;
  scl = 1;
  _nop_();
  _nop_();
  sda = 1;
  scl = 0;
	//delay(10);
}

void write(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)	
	{
		_nop_();
    _nop_();
		scl=0;
		sda=(dat&0x80>>i)?1:0;
		scl=1;
	}
}

void ack(void)
{
	scl=0;
	sda=1;
	scl=1;
	_nop_();
  _nop_();
	scl=0;
}

void noack(void)
{
	scl=0;
	sda=1;
	scl=1;
}

unsigned char read()
{
	unsigned char i,buff=0;
	sda=1;

	for(i=0;i<8;i++)
	{
		scl=1;
		_nop_();
    _nop_();
		if(sda)
		buff|=(0x80>>i);
		scl=0;
	}
	return buff;
}

void save_i2c(char dev,char addr,char ch)
{
	start();
	write(dev);
	ack();
		_nop_();
    _nop_();
	write(addr);
	ack();
	_nop_();
    _nop_();
	write(ch);
	ack();
	_nop_();
  _nop_();
	stop();
	delay(10);
}

char read_i2c(char dev,char addr)
{
	unsigned char buff;
	start();
	write(dev);
	ack();
	_nop_();
    _nop_();
	write(addr);
	ack();
	_nop_();
    _nop_();
	start();
	write(dev|1);
	ack();
	_nop_();
    _nop_();
	buff=read();
	noack();
	stop();
		_nop_();
    _nop_();
	return buff;
}

