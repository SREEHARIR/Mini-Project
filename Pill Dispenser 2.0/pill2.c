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
