/*Transmitter*/

#include<reg52.h>
#include<stdio.h>
#include<string.h>
#include<intrins.h>

sbit RS = P1^2;
sbit EN = P1^1;
sbit D0 = P0^0;
sbit D1 = P0^1;
sbit D2 = P0^2;
sbit D3 = P0^3;
sbit D4 = P0^4;
sbit D5 = P0^5;
sbit D6 = P0^6;
sbit D7 = P0^7;

sbit sw=P2^0;

unsigned char Command_CMGF[]="AT+CMGF=1\r";
unsigned char CtrlZ=0x1A;
//unsigned char Command_CMGS[]={"AT+CMGS =+919446309596\r"};//,"AT+CMGS =+919446309596\r","AT+CMGS =+919446309596\r"};
unsigned char Command_AT[]="AT\r";
//unsigned char msg02[]="Hello!";

unsigned char str[60];
int energy=1,k=0,charge=0;
//unsigned char ascii[10]={'0','1','2','3','4','5','6','7','8','9'};
//unsigned char flag_mode=1,flag1=1,flag2=1,flag3=1,flag4=1,flag5=1,flag6=1,flag7=1,flag8=1,flag9=1;
//unsigned int n=0,k=0,n1=0,k1=0,

//void lcd_enable(void);
void delay(unsigned int count);
//void lcd_cmd(unsigned char com);
//void lcd_string_disp(unsigned char *disp);
//void lcd_init(void);
//void lcd_char_disp(unsigned char disp);
//void lcd_number_display(unsigned char number);
//void display_1(void);
//void display_2(void);
//void control_lights(void);
void gsm_init(void);
void gsm_set(void);
void gsm_tx(unsigned char serialdata);
void gsm_rx(void);
void gsm_tx_str(unsigned char *ref);
void int_to_str(int num);
void init_isr(void);
void disable_isr(void);
void enable_isr(void);
void str_to_ascii(unsigned char *ref);
void sendsms(unsigned char *ref);

void delay(unsigned int count)
{
	int j;
	for(k=0;k<count;k++)
		for(j=0;j<1275;j++);
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
  delay(1);
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
	delay(20);
	///////////// Reset process from datasheet /////////
  Lcd_Cmd(0x30);
	delay(5);
  Lcd_Cmd(0x30);
	delay(11);
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
	int k;
	for(k=0;a[k]!='\0';k++)
	 Lcd_Write_Char(a[k]);
}

void gsm_init(void)		 // INITIALIZE SERIAL PORT
{
	SCON = 0x50;		// SERIAL MODE 1, 8-DATA BIT 1-START BIT, 1-STOP BIT, REN ENABLED
	TMOD |= 0x20;		// Timer 1 IN MODE 2-AUTO RELOAD TO GENERATE BAUD RATE
	TH1 = 0xFD;	// LOAD BAUDRATE TO TIMER REGISTER
	TL1 = 0xFD;
	ES=1;
	EA=1;
	TR1 = 1;		// START TIMER
					
	gsm_set();
}

void gsm_set(void)
{
	//gsm_tx_str("AT+CMGF=1%c");
	//strcpy(str,"AT+CMGF=?\r");
	//strcpy(str,"ABCDEFGHIJKLMNOPQRSTUVWXYZ\rabcdefghijklmnopqrstuvwxyz\r");
	//str_to_ascii(str);
	gsm_tx_str(Command_AT);
	delay(200);
	gsm_tx_str(Command_CMGF);
	delay(200);
	gsm_tx_str("AT+CMGS =+919446309596\r\n");
	delay(200);
	gsm_tx_str("Hi! This number has been registered for Electricity Billing\n");
	gsm_tx(CtrlZ); 
	delay(5);
	return;
}

void gsm_tx(unsigned char serialdata)
{
	SBUF = serialdata;			// LOAD DATA TO SERIAL BUFFER REGISTER
	while(TI == 0);				// WAIT UNTIL TRANSMISSION TO COMPLETE
	TI = 0;		// CLEAR TRANSMISSION INTERRUPT FLAG
}

//void gsm_rx()     // Function to read the response from GSM
//{     
//  while(RI == 0);   // Wait until the byte received  
//  //delay(200);
//	str[k]=SBUF;    //storing byte in str array
//  Rk=0;           //clear RI to receive next byte
//}

void gsm_tx_str(unsigned char *ref)
{
	
	while(*ref!='\0')
	{
		gsm_tx(*ref);
		ref++;
	}
	//gsm_tx(13);
	//gsm_tx(CtrlZ);
}

//void gsm_sms_num(unsigned char *ref)
//{
//	int k=0;
//	Lcd_Set_Cursor(2,11); 
//	Lcd_Write_String("To :");
//	while(*ref!='\0')
//	{
//		str[k]=*ref;
//		k++;
//		ref++;
//	}
//	str[k]='\0';
//	Lcd_Write_String(str);
//	delay(50);
//}

void sendsms(unsigned char *ref)
{
	unsigned char command[]="AT+CMGS=+91xxxxxxxxxx\r";
	k=0;
	while(*ref!='\0')
	{
		str[k]=*ref;
		ref++;
		k++;
	}
	str[k]=*ref;
	Lcd_Clear();
	Lcd_Set_Cursor(1,0); 
	Lcd_Write_String("Sending SMS To :");
	Lcd_Set_Cursor(2,0);
	Lcd_Write_String(str);
	
	strcpy(command,"AT+CMGS=");
	strcat(command,str);
	strcat(command,"\r\n");
	gsm_tx_str(command);
	gsm_tx_str("Your Electricity Energy Consumption is : ");
	int_to_str(energy);
	gsm_tx_str(str);
	gsm_tx_str(" and your Electricity Bill Amount is : " );
	int_to_str(charge);
	gsm_tx_str(str);
}

void int_to_str(int num)
{
	int size=0,k=0,temp;
	while(num!=0)
	{
		str[k]=num%10 + 48;
		num=num/10;
		k++;	
	}
	str[k]='\0';
	for(k=0;str[k]!='\0';k++);
	size=k-1;
	for(k=0;k<((size+1)/2);k++)
	{
		temp = str[k];
		str[k] = str[size-k];
		str[size-k] = temp;
	}
}

//void str_to_ascii(unsigned char *ref)
//{
//	int size=0;
//	for(k=0;*ref!='\0';k++)
//	{
//		str[k]=*ref;
//		ref++;
//	}
//	str[k]='\0';
//	size=k-1;
//	for(k=0;str[k]!='\0';k++)
//	{
//		//if(str[k]==' ')
//		{
//			//str[k]=str[k] + 0x20;
//		}
//		//else if(str[k]>=0x0a)
//		{
//			//str[k]=str[k] + 0x80;//- 0xBF;
//		}
//		//str[k]=str[k]-48;
//	}
//}

void ISR_INT1(void) interrupt 2
{
	energy++;
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

/*
void main()
{
	BIT1=0;
	BIT2=0;
	BIT3=0;
	BIT4=0;
	Lcd_Init();
  gsm_init();
	//gsm_rx();
	//isr_init();
	
  Lcd_Clear();
	Lcd_Set_Cursor(1,4);
	Lcd_Write_String("WELCOME");
	delay(30);
	
  Lcd_Clear();
	Lcd_Set_Cursor(1,1);
	Lcd_Write_String("ENERGY METER");
	Lcd_Set_Cursor(2,1);
	Lcd_Write_String("BILLING Tx");
	delay(30);
	while(1)
	{
		Lcd_Clear();
		Lcd_Set_Cursor(1,2);
		Lcd_Write_String("ENERGY METER");
		Lcd_Set_Cursor(2,1);
		Lcd_Write_String("Energy : ");
		int_to_str(energy); 
		Lcd_Write_String(str);
		delay(300);
		if(sw_2==1)
		{
			while(sw_2==1);
			Lcd_Clear();
			Lcd_Set_Cursor(1,1);
			Lcd_Write_String("SENDING SMS....");
			delay(30);
			//disable_isr();
//			gsm_sms_num("9446309596");
//			int_to_str(energy);
			str_to_ascii(str);
//			gsm_tx_str(str);
	//		energy=1;
	//		enable_isr();
		}
	}
}
*/
void main()
{
	//BIT1=0;
	//BIT2=0;
	//BIT3=0;
	//BIT4=0;
	Lcd_Init();
  gsm_init();
	//gsm_rx();
	isr_init();
	
  Lcd_Clear();
	Lcd_Set_Cursor(1,4);
	Lcd_Write_String("WELCOME");
	delay(200);

	k=0;
		Lcd_Clear();
		Lcd_Set_Cursor(1,0);
    Lcd_Write_String("GSM Electricity Billing");
    delay(200);
    Lcd_Clear();            //    Clear LCD screen
    delay(10);                                     
    //gsm_tx('A');              // Sending 'A' to GSM module
    //Lcd_Write_Char('A');                 
    //delay(1);
    //gsm_tx('T');            // Sending 'T' to GSM module
    //Lcd_Write_Char('T');                 
    //delay(1);
    //gsm_tx(0x0d); 
		//gsm_tx(0x1A);          // Sending carriage return to GSM module       
		//gsm_tx_str(Command_AT);	
		//gsm_tx(0x1A);
    //Lcd_Write_String(Command_AT);                 		
   // delay(50);
    while(1)
    {
/*
      Lcd_Write_Char('F');	  
			gsm_rx();
			Lcd_Write_Char('F');
			Lcd_Write_Char(str[k]);    
        if(str[k-1]=='O' && str[k]=='K'){
            Lcd_Write_Char(0x20);                      // Write 'Space'
            Lcd_Write_Char(str[k-1]);
            Lcd_Write_Char(str[k]);
            break; 
        }
        k=k+1;
				if(k>50)
					k=0;
    
*/
			charge = energy * 5;
			Lcd_Set_Cursor(1,0);
			Lcd_Write_String("Energy : ");
			int_to_str(energy);
			Lcd_Write_String(str);
			Lcd_Set_Cursor(2,0);
			Lcd_Write_String("Charge : ");
			int_to_str(charge);
			Lcd_Write_String(str);
			if(sw==1)
				{
					disable_isr();
					while(sw==1);
					Lcd_Clear();
					Lcd_Set_Cursor(1,0);
					Lcd_Write_String("SENDING SMS....");
					delay(30);
					sendsms("+919446309596");         // Phone number of USER
					sendsms("1234567b89");         // Phone number of KSEB
					energy=1;
					Lcd_Clear();
					enable_isr();
				}
		}
	}