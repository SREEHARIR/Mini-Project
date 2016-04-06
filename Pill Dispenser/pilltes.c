#include<reg52.h> 
#include<intrins.h>
#include<lcd.h>
//Memory Module Connections
sbit sda = P1 ^ 0;
sbit scl = P1 ^ 1;
//sbit led=P0^3;
//sbit ack;

//LCD Module Connections
sbit RS = P0^4;
sbit EN = P0^5;
//sbit D0 = P2^0;
//sbit D1 = P2^1;
//sbit D2 = P2^2;
//sbit D3 = P2^3;
sbit D4 = P2^4;
sbit D5 = P2^5;
sbit D6 = P2^6;
sbit D7 = P2^7;
//End LCD Module Connections

sbit sw_1 = P1 ^ 6;
sbit sw_2 = P1 ^ 7;


unsigned char str[60];
int alarms=0;
/*
unsigned char Command_CMGF[]="AT+CMGF=1\r";
unsigned char CtrlZ=0x1A;
unsigned char Command_CMGS[]="AT+CMGS =+919446309596\r";
unsigned char Command_AT[]="AT\r";
*/
void int_to_str(int num);
void init_isr(void);
void disable_isr(void);
void enable_isr(void);
/* TODO*/
void time(void);
void alarm(void);
void set_time(void);
void stepper(int pill,int motor);


void delay(int a) 
{
    int i,j;
    for (i = 0; i < a; i++) 
        for (j = 0; j < 1000; j++);
}

void int_to_str(int num)
{
	int size,i=0,temp;
	while(num!=0)
	{
		str[i]=num%10 + 48;
		num=num/10;
		i++;	
	}
	str[i]='\0';
	size=0;
	for(i=0;str[i]!='\0';i++);
	size=i-1;
	for(i=0;i<((size+1)/2);i++)
	{
		temp = str[i];
		str[i] = str[size-i];
		str[size-i] = temp;
	}
}
/*
-----------------------------------------------------------
void aknowledge() //acknowledge condition
{
    scl = 1;
    _nop_();
    _nop_();
    scl = 0;
}

void start() //start condition
{
    sda = 1;
    scl = 1;
    _nop_(); //No operation
    _nop_();
    sda = 0;
    scl = 0;
}

void stop() //stop condition
{
    sda = 0;
    scl = 1;
    _nop_();
    _nop_();
    sda = 1;
    scl = 0;
}
-------------------------------------------------------------------
*/
/*
void send_byte(unsigned char value) //send byte serially
{
    unsigned int i;
    unsigned char send;
    send = value;
    for (i = 0; i < 8; i++) {
        sda = send / 128; //extracting MSB
        send = send << 1; //shiftng left
        scl = 1;
        _nop_();
        _nop_();
        scl = 0;
    }
    ack = sda; //reading acknowledge
    sda = 0;
}

unsigned char read_byte() //reading from EEPROM serially
{
    unsigned int i, read;
    sda = 1;
    read = 0;
    for (i = 0; i < 8; i++) 
    {
        read = read << 1;
        scl = 1;
        _nop_();
        _nop_();
        if (sda == 1)
            read++;
        scl = 0;
    }
    sda = 0;
    return read; //Returns 8 bit data here
}

void save() //save in EEPROM
{
    unsigned char input1, input2, input3, input4;
    input1 = 8;
    input2 = 0;
    input3 = 5;
    input4 = 1;
    start();
    send_byte(0xA0); //device address
    aknowledge();
    send_byte(0x00); //word address
    aknowledge();
    send_byte(input1); //send data
    aknowledge();
    send_byte(input2);
    aknowledge();
    send_byte(input3); //send data
    aknowledge();
    send_byte(input4);
    aknowledge();
    stop();

    Lcd4_Cmd(0x86);
    input1 = input1 + 48;
    lcd_data(input1);
    lcd_command(0x87);

    input2 = input2 + 48;
    lcd_data(input2);
    lcd_command(0x88);

    input3 = input3 + 48;
    lcd_data(input3);
    lcd_command(0x89);

    input4 = input4 + 48;
    lcd_data(input4);
    aknowledge();
}

void Read() 
{
    unsigned char h, i, j, k, write;
    start();
    send_byte(0xA0);
    aknowledge();
    send_byte(0x00);
    aknowledge();
    start();
    send_byte(0xA1); //device address
    aknowledge();

    h = read_byte();
    aknowledge();

    i = read_byte();
    aknowledge();

    j = read_byte();
    aknowledge();

    k = read_byte();
    aknowledge();
    stop();



    write = h + 48;
    lcd_command(0xC6);
    lcd_data(write);



    lcd_command(0xC7);
    write = i + 48;
    lcd_data(write);

    lcd_command(0xC8);
    write = j + 48;
    lcd_data(write);

    lcd_command(0xC9);
    write = k + 48;
    lcd_data(write);
    aknowledge();
}
*/

/*

void Lcd_Delay(int a)
{
    int j;
    int i;
    for(i=0;i<a;i++)
    {
        for(j=0;j<100;j++)
        {
        }
    }
}

void Lcd4_Port(char a)
{
	if(a & 1)
		D4 = 1;
	else 
		D4 = 0;
	
	if(a & 2)
		D5 = 1;
	else
		D5 = 0;
	
	if(a & 4)
		D6 = 1;
	else
		D6 = 0;
	
	if(a & 8)
		D7 = 1;
	else
		D7 = 0;
}
void Lcd4_Cmd(char a)
{ 
	RS = 0;             // => RS = 0
	Lcd4_Port(a);
	EN  = 1;             // => E = 1
  Lcd_Delay(5);
  EN  = 0;             // => E = 0
}

void Lcd4_Clear()
{
	Lcd4_Cmd(0);
	Lcd4_Cmd(1);
}

void Lcd4_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 1)
	{
	  temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
	else if(a == 2)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
}

void Lcd4_Init()
{
	Lcd4_Port(0x00);
	Lcd_Delay(200);
	///////////// Reset process from datasheet /////////
  Lcd4_Cmd(0x03);
	Lcd_Delay(50);
  Lcd4_Cmd(0x03);
	Lcd_Delay(110);
  Lcd4_Cmd(0x03);
  /////////////////////////////////////////////////////
  Lcd4_Cmd(0x02);    
	Lcd4_Cmd(0x02);
  Lcd4_Cmd(0x08); 	
	Lcd4_Cmd(0x00); 
	Lcd4_Cmd(0x0C);     
  Lcd4_Cmd(0x00);    
  Lcd4_Cmd(0x06);   
}

void Lcd4_Write_Char(char a)
{
   char temp,y;
   temp = a&0x0F; 
   y = a&0xF0;	
	 RS = 1;             // => RS = 1
   Lcd4_Port(y>>4);             //Data transfer
	 EN = 1;
	 Lcd_Delay(5);
	 EN = 0;
	 Lcd4_Port(temp);
	 EN = 1;
	 Lcd_Delay(5);
	 EN = 0;
}

void Lcd4_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	 Lcd4_Write_Char(a[i]);
}
*/

void str_to_ascii(unsigned char *ref)
{
	int i=0,size=0;
	for(i=0;*ref!='\0';i++)
	{
		str[i]=*ref;
		ref++;
	}
	str[i]='\0';
	size=i-1;
	for(i=0;str[i]!='\0';i++)
	{
		//if(str[i]==' ')
		{
			//str[i]=str[i] + 0x20;
			//return(temp+0x20);
		}
		//else if(str[i]>=0x0a)
		{
			//str[i]=str[i] + 0x80;//- 0xBF;
			//str[i]=str[i]-0x20;
			//return(temp+0x80);
		}
		//str[i]=str[i]-48;
	}
}

void ISR_INT1(void) interrupt 2
{
	//energy++;
}
void isr_init(void)
{
	IE = 0x84;
	IT1 = 1;
}
void disable_isr(void)
{
	IE=0x04;
}

void enable_isr(void)
{
	IE=0x84;
}


void main() 
{
    int i;
    Lcd4_Init();
    Lcd4_Clear();
    while (1) 
    {
        Lcd4_Set_Cursor(1, 0);
        Lcd4_Write_String("Pill Dispenser 2");
				delay(100);
				
//        save();
//        Read();

    }
}






