#include<reg52.h>
#include<string.h>
#define display_port P0      //Data pins connected to port 2 on microcontroller
//sbit rs = P3^2;  //RS pin connected to pin 2 of port 3
sbit rw = P0^3;  // RW pin connected to pin 3 of port 3
sbit rs = P1^2;
sbit e = P1^1;
//sbit e =  P3^4;  //E pin connected to pin 4 of port 3

int k;

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

unsigned char str[26];

void GSM_init()            // serial port initialization 
{
    TMOD=0x20;            // Timer 1 selected, Mode 2(8-bit auto-reload mode)
    TH1=0xfd;            // 9600 baudrate
    SCON=0x50;            // Mode 1(8-bit UART), receiving enabled
    TR1=1;                // Start timer
}
void msdelay(unsigned int time)  // Function for creating delay in milliseconds.
{
    unsigned m,n ;
    for(m=0;m<time;m++)    
    for(n=0;n<1275;n++);
}

void GSM_write(unsigned char ch)    // Function to send commands to GSM
{
    SBUF=ch;        // Put byte in SBUF to send to GSM
    while(TI==0);        //wait until the byte trasmission
    TI=0;            //clear TI to send next byte.      
}
 void GSM_read()    interrupt 4 // Function to read the response from GSM
 {     
	 while(RI==0);   // Wait until the byte received  
   Lcd_Write_Char('.'); 
	 str[k]=SBUF;  //storing byte in str array
   RI=0;           //clear RI to receive next byte
	 Lcd_Write_Char(str[k]);
		k++;
	 
 }
 
void gsm_tx_str(unsigned char *ref)
{
	
	while(*ref!='\0')
	{
		GSM_write(*ref);
		ref++;
	}
	//gsm_tx(13);
	GSM_write(0x1a);
}
void main()
{        
    k=0;
    Lcd_Init();                            
    GSM_init();                
    msdelay(200);
Lcd_Write_String("Interfacing GSM with 8051");
    msdelay(200);
    Lcd_Clear();            //    Clear LCD screen
    msdelay(10);                                     
    Lcd_Write_Char('A');                 
    msdelay(1);
    Lcd_Write_Char('T');                 
    msdelay(1);
		GSM_write('A');              // Sending 'A' to GSM module
    GSM_write('T');            // Sending 'T' to GSM module
  GSM_write('\n');
GSM_write('\r');  
	GSM_write(0x0d);          // Sending carriage return to GSM module            
  GSM_write(0x1A);        
	gsm_tx_str("AT\r\n");          // Sending 'T' to GSM module
  //GSM_write(0x0d);          // Sending carriage return to GSM module            
  //GSM_write(0x1A);   
	//msdelay(50);
	while(1)
  {          
//    GSM_read();
		//Lcd_Write_Char(str[k]);
		if(str[k-1]=='O' && str[k]=='K'){
         Lcd_Write_Char('*');
				Lcd_Write_Char('O');//str[k-1]);
          Lcd_Write_Char('K');//str[k]);
          //break; 
      }
		Lcd_Set_Cursor(2,0);                      // Write 'Space'
		Lcd_Write_Char('.');
    k++;                                            
  }
}