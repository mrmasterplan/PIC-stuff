#define _VERSION_ "V0.0.3"
/****************************************************************
*                      main.c
*
*                     Pulse232
*
* Monitors Pulse rate coming in to Log errors for XRAY
* When under a preset rate, it outputs the rate to RS232 with 
* a time stamp.
*
* Input Range = 0Hz -> 255Hz in 1Hz steps - Accuracy +/- 1Hz.
*
* Uses a PCF8583 I2C Calander Clock for 1Hz INT and time of day.
* Backed up by a 1.0F Capacitor
*
* IMPORTANT PINS:
* 
* RA4/T0CKI - Pulse Input    - INPUT
* RB0/INT   - 1Hz Interrupt  - INPUT
* RB2       - SCL            - OUTPUT
* RB3       - SDA            - BI-DIRECTIONAL
* RA0       - TXD            - OUTPUT
* RA1       - RXD            - INPUT
*
*
* Target Processor: PIC16F84
*
* Compile Options: Full Optimisation
*
*
* Author: Michael Pearce
*         Electronics Workshop
*         Chemistry Department
*         University of Canterbury
*
* Started: 14 November 2001
*
*****************************************************************
*                    Version Information
*****************************************************************
* Version 0.0.3 - 19 November 2001
* Look at fixing Set up mode input
*****************************************************************
* Version 0.0.2 - 19 November 2001
* Added Start up delay
* Changed 80000000 to 8000000 to give correct clok for serial
* Change baud to 9600
* Changed I2C Address
*****************************************************************
* Version 0.0.1 - 15 november 2001
* Work on the Clock reading/Writing.
* Did interrupt routine
* Theoretically finished it... time to try it and find bugs.
*****************************************************************
* Version 0.0.0 - 14 November 2001
* Start of Project 
****************************************************************/
#include <pic.h>


#define XTAL_FREQ 8MHZ

/* Serial Port Settings - must be b4 MikesLib */
#define	XTAL   8000000        // Crystal frequency in Hz
#define SERIALSOFTWARE         // Indicate type of serial output
#define TxPort PORTA           // Pin on Port B
#define RxPort PORTA
#define TxBit  0               // Pin Number to use
#define RxBit  1
#define TxTris TRISA           // Tris Register for Pin
#define RxTris TRISA
#define BRATE  9600            // Baud rate required

#include "mikeslib.c"


#include "delay.c"


/* I2C Ports Etc */
static bit SCL      @ PORTBIT(PORTB,2);
static bit SCL_TRIS @ PORTBIT(TRISB,2);

static bit SDA      @ PORTBIT(PORTB,1);
static bit SDA_TRIS @ PORTBIT(TRISB,1);

#include "m_i2c_1.c"




#define I2CADDRESS 0xA0     /* Address of the Clock on the I2C Bus */
#define ADDR_LIMIT 0x10     /* Address of LIMIT Data in Clock Chip */



/* Other I/O Ports */

static bit Pulse   @ PORTBIT(PORTA,4);
static bit Pulse_T @ PORTBIT(TRISA,4);

static bit OneHz   @ PORTBIT(PORTB,0);
static bit OneHz_T @ PORTBIT(TRISB,0);


static bit Switch  @ PORTBIT(PORTA,2);
static bit Switch_T@ PORTBIT(TRISA,2);

/* Globals */

bit OneHz_Flag;
char CurrentTime[9]={_VERSION_};

char Hour,Minute,Second,Limit;

char I2CBuffer[6];

char PulseCount;

/* Functions */
void SetupTime(void);
void GetTime(void);
void puts(char *str);
char GetNumber(void);
void WriteTime(void);
void Time2String(void);
void SetLimit(char c);
void GetLimit(void);
void PrintNumber(char num);

/****************************************************************
*                        main
****************************************************************/
void main (void)
{
 OPTION=0x7F; /* PullUp / INT Rise / T0 Ext / T0 h->l / pre->wdt / pre=128 */

 TRISA=0xFF;
 TRISB=0xFF;

 /* Start Up Delay */
 DelayMs(100);
 
 /* Set up soft serial */
 InitSerial();

 /* Set Up I2C */ 
 i2cstart();
 i2cstop();

 /* Get Limit from I2C */
 GetLimit();

 /* Print Version (Loaded in CurrentTime String) */
 puts(CurrentTime);
 putch('-');

 /* Get And Print the Current Time */
 GetTime(); 
 puts(CurrentTime);
 putch('-');

 /* Print the Current Limit Setting */
 PrintNumber(Limit);
 
 putch('\r');putch('\n');
 
 /* Check if In Time Set Mode */
 if(Switch==0) SetupTime();

 
 /* Set Up timer and Interrupts */
 INTCON=0;
 INTE=1;
 GIE=1;


 CLRWDT();
 /* Wait till next INT to stop a false reading */
 OneHz_Flag=0;
 while(OneHz_Flag==0);
 OneHz_Flag=1;


 /* Main Program */
 while(1)
 {
  CLRWDT();
  /* Wait for 1Hz interrupt */
  while(OneHz_Flag==0); 
  OneHz_Flag=0;

  /* See if need to send to serial  */
  if(PulseCount < Limit)
  {
   /* Read Current time from I2C */
   GetTime();

   /* Output Time to serial */
   puts(CurrentTime);
   
   /* Comma Seperate for import into spreadsheet */
   putch(',');

   /* Convert and send PulseCount */
   PrintNumber(PulseCount);
   putch('\r');putch('\n');

  }
 }

}
/************ END OF main ****************/


/****************************************************************
*                      SetupTime
*
* Uploads time from serial to I2C Calendar Clock
*
* RS232 Format:
*  "Hxx" Set Hours to xx
*  "Mxx" Set Minutes to xx
*  "Sxx" Set Seconds to xx
*  "Lxx" Set Limit to xx
*  "E"   END config
*  
* Note: 2 Digits must follow the command!!
* A incorrect command will display an 'X'
*
*  
****************************************************************/
void SetupTime(void)
{
 char done=0,c;

 while(!done)
 {
  c=getch();
  putch(c);
  switch(c)
  {
   case 'H':
    Hour=GetNumber();
    break;

   case 'M':
    Minute=GetNumber();
    break;

   case 'S':
    Second=GetNumber();
    break;

   case 'L':
    c=GetNumber();
    SetLimit(c);
    break;

   case 'E':
    WriteTime();
    done=1;
    break;

   default:
    putch('X');
    break;
   
  }
 }

}
/************ END OF SetupTime ****************/


/****************************************************************
*                      GetTime
*
* Read the time from I2C, and convert into "hhmm\0" format
****************************************************************/
void GetTime(void)
{
 I2CBuffer[0]=0x02;                /* Seconds Position */
 I2C_Send(I2CADDRESS,I2CBuffer,1); /* Tell to read from Seconds */
 I2C_Read(I2CADDRESS,I2CBuffer,3); /* Read Seconds,Minutes, hrs */

 Second=I2CBuffer[0];
 Minute=I2CBuffer[1];
 Hour=I2CBuffer[2]&0x3F;   /* Clear out 12/24Hr mode bit and AM/PM bit */
 
 Time2String(); 	
}
/************ END OF GetTime ****************/



/****************************************************************
*                          puts
****************************************************************/
void puts (char *str)
{
 while(*str!=0)
 {
  putch(*str);
  str++;
 }
}
/************ END OF puts ****************/



/****************************************************************
*                      GetNumber
*
* Reads next 2 characters from serial and converts them to 
* 8bit BCD that is returned
****************************************************************/
char GetNumber(void)
{
 unsigned char temp,res;

 res=0;
 
 temp=getch();
 putch(temp);
 if(temp >= '0' && temp <='9')
 {
  res=temp-'0';
  res=res<<4;
  res &=0xF0;
 }

 temp=getch();
 putch(temp);
 if(temp >= '0' && temp <='9')
 {
  temp=temp-'0';
  temp &= 0x0F;
  res |= temp;
 }

 return(res);
}
/************ END OF GetNumber ****************/



/****************************************************************
*                      WriteTime
*
* Writes the current set time to the Clock 
****************************************************************/
void WriteTime(void)
{
 I2CBuffer[0]=0x00;      /* Start at Command Reg */
 I2CBuffer[1]=0x00;      /* Command Reg - Clock Mode with 1Hz int */
 I2CBuffer[2]=0x00;      /* Set 100ths to 0 */
 I2CBuffer[3]=Second;    /* Seconds */
 I2CBuffer[4]=Minute;    /* Minutes */
 I2CBuffer[5]=Hour|0x80; /* Ensure Hrs are in 24 Hour mode */

 I2C_Send(I2CADDRESS,I2CBuffer,6); /* Write Data to Clock Chip */
 

}
/************ END OF WriteTime ****************/



/****************************************************************
*                        Time2String
*
* Converts The BCD Time into a human readable string
****************************************************************/
void Time2String(void)
{
 char temp;

 /* Hours */
 temp=Hour&0xF0;
 temp=temp>>4;
 CurrentTime[0]=temp+'0'; 
 temp=Hour&0x0F;
 CurrentTime[1]=temp+'0';

 /* Break Between Hrs and Sec */
 CurrentTime[2]=':';

 /* Minutes */
 temp=Minute&0xF0;
 temp=temp>>4;
 CurrentTime[3]=temp+'0'; 
 temp=Minute&0x0F;
 CurrentTime[4]=temp+'0';

 /* Break between Min and Sec */
 CurrentTime[5]=':';
 
 
 /* Seconds */
 temp=Second&0xF0;
 temp=temp>>4;
 CurrentTime[6]=temp+'0'; 
 temp=Second&0x0F;
 CurrentTime[7]=temp+'0';

 /* Terminate the String */
 CurrentTime[8]=0;  

}
/************ END OF Time2String  ****************/




/****************************************************************
*                   InterruptRoutine
****************************************************************/
void interrupt InterruptRoutine(void)
{
 /* Check for 1Hz Interrupt */
 if(INTF)
 {
  /* Record the current count and Clear it */
  PulseCount=TMR0;
  TMR0=0;
  OneHz_Flag=1; /* Indicate that there is something to do */
  INTF=0;  
 }
}

/************ END OF InterruptRoutine  ****************/




/****************************************************************
*                      GetLimit
*
* Reads Limit from I2C Clock Chip
****************************************************************/
void GetLimit(void)
{
 I2CBuffer[0]=ADDR_LIMIT;
 I2C_Send(I2CADDRESS,I2CBuffer,1); /* Select Addr to read from */
 I2C_Read(I2CADDRESS,&Limit,1);     /* Read Limit */
}
/************ END OF GetLimit ****************/




/****************************************************************
*                       SetLimit
*
* Writes Limit to I2C Clock Chip
****************************************************************/
void SetLimit(char c)
{
 Limit=c & 0x0F;   /* Put lox 4 bits in */
 c = (c >>4) *10;  /* Convert High 4 bits to 10s num */
 Limit+=c;         /* Add 10s to limit */
 I2CBuffer[0]=ADDR_LIMIT;
 I2CBuffer[1]=Limit;
 I2C_Send(I2CADDRESS,I2CBuffer,2); /* Write Limit to Buffer */

}
/************ END OF  SetLimit ****************/

/****************************************************************
*                      PrintNumber
*
* Converts char number to string and prints it out
****************************************************************/
void PrintNumber(char num)
{
 char temp;

 if(num > 100)
 {
  temp=num/100;
  putch(temp+'0');
  num-=(temp*100);
 }
 else
 {
  putch('0');
 }

 if(num >10)
 {
  temp=num/10;
  putch(num+'0');
  num-=(temp*10);
 }
 else
 {
  putch('0');
 }

 putch(num+'0');
 
}
/************ END OF  PrintNumber ****************/

