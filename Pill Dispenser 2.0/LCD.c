#include "Includes.h"

void ToggleEpinOfLCD(void)
{
	E = 1;                // Give a pulse on E pin
	__delay_us(E_Delay);  // so that LCD can latch the
	E = 0;                // data from data bus
	__delay_us(E_Delay); 	
}

void WriteDataToLCD(char t)
{
   RS = 1;             // This is data

   P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
   P2 |= (t&0xF0);     // Write Upper nibble of data
   ToggleEpinOfLCD();  // Toggle E pin to send data 

   P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
   P2 |= ((t<<4)&0xF0);// Write Lower nibble of data
   ToggleEpinOfLCD();  // Toggle E pin to send data 
}


void WriteCommandToLCD(int z)
{
   RS = 0;             // This is command

   P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
   P2 |= (z&0xF0);     // Write Upper nibble of data
   ToggleEpinOfLCD();  // Toggle E pin to send data 

   P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
   P2 |= ((z<<4)&0xF0);// Write Lower nibble of data
   ToggleEpinOfLCD();  // Toggle E pin to send data 
}

void InitLCD(void)
{
	RS = 0;		 	// Make pin zero
	E  = 0;			// Make Pin zero					

  ///////////// Reset process from datasheet /////////
     __delay_us(15000);

	 P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
	 P2 |= 0x30;    	   // Write 0x3
     ToggleEpinOfLCD();    // Toggle E pin to send data 

     __delay_us(4500);

	 P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
	 P2 |= 0x30;    	   // Write 0x3
     ToggleEpinOfLCD();    // Toggle E pin to send data 

     __delay_us(300);

	 P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
	 P2 |= 0x30;    	   // Write 0x3
     ToggleEpinOfLCD();    // Toggle E pin to send data 

     __delay_us(650);

	 P2 &= 0x0F;		   // Make P2.4 to P2.7 zero
	 P2 |= 0x20;    	   // Write 0x2
     ToggleEpinOfLCD();    // Toggle E pin to send data 

	 __delay_us(650);

  /////////////////////////////////////////////////////
   WriteCommandToLCD(0x28);    //function set
   WriteCommandToLCD(0x0c);    //display on,cursor off,blink off
   WriteCommandToLCD(0x01);    //clear display
   WriteCommandToLCD(0x06);    //entry mode, set increment
}



void ClearLCDScreen(void)       // Clear the Screen and return cursor to zero position
{
	WriteCommandToLCD(0x01);    // Clear the screen
	__delay_us(2000);           // Delay for cursor to return at zero position
}


void WriteStringToLCD(const char *s)
{
	while(*s)	
		WriteDataToLCD(*s++); 
}

void SetLcdCursor(char a, char b)
{
	if(a == 1)
	  WriteCommandToLCD(0x80 + b);
	else if(a == 2)
		WriteCommandToLCD(0xC0 + b);
}

void DisplayTimeToLCD( unsigned char* pTimeArray )   // Displays time in HH:MM:SS AM/PM format
{
	ClearLCDScreen();      // Move cursor to zero location and clear screen
	SetLcdCursor(2,0);
	// Display Hour
	WriteDataToLCD( (pTimeArray[2]/10)+0x30 );
	WriteDataToLCD( (pTimeArray[2]%10)+0x30 );

	//Display ':'
	WriteDataToLCD(':');

	//Display Minutes
	WriteDataToLCD( (pTimeArray[1]/10)+0x30 );
	WriteDataToLCD( (pTimeArray[1]%10)+0x30 );

	//Display ':'
	WriteDataToLCD(':');

	//Display Seconds
	WriteDataToLCD( (pTimeArray[0]/10)+0x30 );
	WriteDataToLCD( (pTimeArray[0]%10)+0x30 );

	//Display Space
	WriteDataToLCD(' ');

	// Display mode
	switch(pTimeArray[3])
	{
	case AM_Time:	WriteStringToLCD("AM");	break;
	case PM_Time:	WriteStringToLCD("PM");	break;

	default: WriteDataToLCD('H');	break;
	}
}




void DisplayDateOnLCD( unsigned char* pDateArray )   // Displays Date in DD:MM:YY @ Day format
{
	SetLcdCursor(2,11);
	
	// Display Date
	WriteDataToLCD( (pDateArray[1]/10)+0x30 );
	WriteDataToLCD( (pDateArray[1]%10)+0x30 );

	//Display '/'
	WriteDataToLCD(':');

	//Display Month
	WriteDataToLCD( (pDateArray[2]/10)+0x30 );
	WriteDataToLCD( (pDateArray[2]%10)+0x30 );

	//Display '/'
	WriteDataToLCD(':');

	//Display Year
	WriteDataToLCD( (pDateArray[3]/10)+0x30 );
	WriteDataToLCD( (pDateArray[3]%10)+0x30 );

	//Display Space
	WriteDataToLCD(' ');

	// Display Day
	switch(pDateArray[0])
	{
	case Monday:	WriteStringToLCD("MON");	break;
	case Tuesday:	WriteStringToLCD("TUE");	break;
	case Wednesday:	WriteStringToLCD("WED");	break;
	case Thursday:	WriteStringToLCD("THU");	break;
	case Friday:	WriteStringToLCD("FRI");	break;
	case Saturday:	WriteStringToLCD("SAT");	break;
	case Sunday:	WriteStringToLCD("SUN");	break;

	default: WriteStringToLCD("???");	break;
	}
}