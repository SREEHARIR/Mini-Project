#include<reg51.h>
#include<intrins.h>

sbit scl=P0^0;
sbit sda=P0^1;

#define LCD P2

sbit rs=P0^4;
sbit rw=P3^1;
sbit en=P0^5;

void delay(int i)
{
int j;
while(i--)
for(j=0;j<1223;j++);
}



void start(void)
{
	/*to start make
	high to low of sda with a clock is high
	*/
	sda=1;
	scl=1;
	 
	 	
	sda=0;
	scl=0;
}

void stop()
{
	/* to stop the i2c
	make an low to high of sda with scl =1*/
	sda = 0;
  scl = 1;
   
   
  sda = 1;
  scl = 0;
	//delay(10);
}

void write(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)	
	{
		 
     
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
		 
     
	write(addr);
	ack();
	 
     
	write(ch);
	ack();
	 
     
	stop();
	delay(10);
}

char read_i2c(char dev,char addr)
{
	unsigned char buff;
	start();
	write(dev);
	ack();
	 
     
	write(addr);
	ack();
	 
     
	start();
	write(dev|1);
	ack();
	 
     
	buff=read();
	noack();
	stop();
		 
     
	return buff;
}



void cmd_LCD(unsigned char value)
{
LCD=value;
rs=0;
rw=0;
en=1;
delay(1);
en=0;
}
void data_LCD(unsigned char value)
{
LCD=value;
rs=1;
rw=0;
en=1;
delay(1);
en=0;
}


void init_LCD()
{
cmd_LCD(0x38); //2 lines and 5*7 matrix
cmd_LCD(0x80); //force curser to begin in the first line
cmd_LCD(0x01); //clr displayyy
cmd_LCD(0x0c);
}

void main()
{

int i;char ch;

init_LCD();
cmd_LCD(0x80);

save_i2c(0xd0,0X00,0x80);
save_i2c(0xd0,0X01,0x59);
save_i2c(0xd0,0X02,0x23);
save_i2c(0xd0,0X03,0x04);
save_i2c(0xd0,0X04,0x20);
save_i2c(0xd0,0X05,0X01);
save_i2c(0xd0,0X06,0x10);
save_i2c(0xd0,0X07,0x10);

for(i=0;i<16;i++)
{
save_i2c(0xa0,i,i+'a');
ch=read_i2c(0xa0,i);
data_LCD(ch) ;
}

save_i2c(0xd0,0X00,0xFF);
save_i2c(0xd0,0X01,0x59);
save_i2c(0xd0,0X02,0x23);
save_i2c(0xd0,0X00,0x00);

while(1)
{
cmd_LCD(0xc0);
	//for(i=0;i<16;i++)
//data_LCD('*');
	ch=read_i2c(0xd0,0X02);
	//data_LCD(ch+48);
data_LCD(ch/16+48);
data_LCD(ch%16+48);
data_LCD('-');
ch=read_i2c(0xd0,0X01);
//data_LCD(ch+48);
	data_LCD(ch/16+48);
data_LCD(ch%16+48);
data_LCD('-');
ch=read_i2c(0xd0,0X00);
//data_LCD(ch+48);
	data_LCD((ch&0x7F)/16+48);
data_LCD(ch%16+48);
}

}