#include <reg51.h>                              //Define 8051 registers
#include<stdio.h>    

void DelayMs(unsigned int);                     //Delay function

 #define stepper P3
sbit buz = P1^4;

//----------------------------------

//     Main Program

//----------------------------------

void ClockwiseA (void)

{

        unsigned int i;

        for (i=0;i<3;i++)

        {

                P3 = 0x01;DelayMs(5);               //Delay 20msec

                P3 = 0x04;DelayMs(5);

                P3 = 0x02;DelayMs(5);

                P3 = 0x08;DelayMs(5);
					DelayMs(20);

        }      

}
void ClockwiseB (void)

{

        unsigned int i;

        for (i=0;i<3;i++)

        {

                P3 = 0x10;DelayMs(5);               //Delay 20msec

                P3 = 0x40;DelayMs(5);

                P3 = 0x20;DelayMs(5);

                P3 = 0x80;DelayMs(5);
					DelayMs(20);

        }      

}

 

void AntiClockwise (void)

{

        unsigned int i;

        for (i=0;i<3;i++)

        {

                P3 = 0x08;DelayMs(5);               //Delay 20msec

                P3 = 0x02;DelayMs(5);

                P3 = 0x04;DelayMs(5);

                P3 = 0x01;DelayMs(5);      

        }

}

 

void main (void)

{
int i;
        P3 = 0;
	P1=0xff;

	//Initialize Port0
				buz = 0x01;
        while(1)                                         //Loop Forever

        {

          P1=0;      
					for( i=0;i<11;i++)
					{
								ClockwiseA ();
            ClockwiseB ();
						DelayMs (100);
			//buz=0;
	
              	
                P3   =      0;
						P1=0xff;
					}
					

             //   AntiClockwise ();

               // DelayMs (100);

                //P3   =      0;

        }

}
/*
void main()
{
	while (1) {
		stepper = 0x08;
		delay();
		stepper = 0x02;
		delay();
		stepper = 0x04;
		delay();
		stepper = 0x01;
		delay();
	}
}

*/
 

//---------------------------------            

//             Delay Function

//---------------------------------

void DelayMs(unsigned int n)

{

unsigned int i,j;

        for(j=0;j<n;j++)

        {

        for(i=0;i<800;i++);

        }

}