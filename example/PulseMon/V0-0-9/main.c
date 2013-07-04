#define _VERSION_ "V0.0.9"
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
* Version 0.0.9 - 4 December 2001
* Add Day Display to output - to distinguish days
*****************************************************************
* Version 0.0.8 - 21 November 2001
* Try to figure out y not in 24 hr mode.
* Found the problem - Needed to CLEAR bit 7 of Hrs not set it!!!
*****************************************************************
* Version 0.0.7 - 20 November 2001
* Increase some buffers
*****************************************************************
* Verison 0.0.6 - 20 November 2001
* Add WDT settings
*****************************************************************
* Verison 0.0.5 - 20 November 2001
* Moved Start up display to seperate Function
* Added "SETUP" display when in setup mode
* Still Dont have Clock Working!!!!
* Swapped I2C Pins round!!!
* Now Read correct number of Bytes
*****************************************************************
* Version 0.0.4 - 20 November 2001
* Need to get I2C Working properly
* Fixed Data -> Decimal Conversion
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
#include <ctype.h>


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
static bit SCL      @ PORTBIT(PORTB,1);
static bit SCL_TRIS @ PORTBIT(TRISB,1);

static bit SDA      @ PORTBIT(PORTB,2);
static bit SDA_TRIS @ PORTBIT(TRISB,2);

#include "m_i2c_1.c"




#define I2CADDRESS 0xA0     /* Address of the Clock on the I2C Bus */
#define LIMIT_ADDR 0x01     /* Address of LIMIT Data in E2PROM */



/* Other I/O Ports */

static bit Pulse   @ PORTBIT(PORTA,4);
static bit Pulse_T @ PORTBIT(TRISA,4);

static bit OneHz   @ PORTBIT(PORTB,0);
static bit OneHz_T @ PORTBIT(TRISB,0);


static bit Switch  @ PORTBIT(PORTA,2);
static bit Switch_T@ PORTBIT(TRISA,2);

/* Globals */

bit OneHz_Flag;
char CurrentTime[16];

char Version[9]={_VERSION_};


char Hour,Minute,Second,Limit,Date,Month,Year;

char I2CBuffer[8];

char PulseCount;

/* Functions */
void SetupTime(void);
void GetTime(void);
void puts(char *str);
char GetNumber(void);
void WriteTime(void);
void Time2String(void);
void SetLimit(unsigned char c);
void GetLimit(void);
void PrintNumber(unsigned char num);
void ShowConfig(void);

/****************************************************************
*                        main
****************************************************************/
void main (void)
{
 
 OPTION=0x7F; /* PullUp / INT Rise / T0 Ext / T0 h->l / pre->wdt / pre=128 */

 TRISA=0xFF;
 TRISB=0xFF;

 CLRWDT();
 
 /* Start Up Delay */
 DelayMs(100);
 
 /* Set up soft serial */
 InitSerial();

 /* Set Up I2C */ 
 i2cstart();
 i2cstop();

 /* Display Version time Etc */
 ShowConfig();

 
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

 putch('S');putch('E');putch('T');putch('U');putch('P');putch(':');

 while(!done)
 {
  c=toupper(getch());
  putch(c);
  switch(c)
  {
   case 'H':              /* Hour */
    Hour=GetNumber();
    break;

   case 'M':              /* Minute */
    Minute=GetNumber();
    break;

   case 'S':              /* Second */
    Second=GetNumber();
    break;

   case 'L':              /* Limit */
    c=GetNumber();
    SetLimit(c);
    break;

   case 'D':              /* Day */
    Date=GetNumber(); 
    Date=Date & 0x3F;
    break;

   case 'N':              /* Month */
    Month=GetNumber();
    Month=Month & 0x1F;
    break;

   case 'Y':              /* Year (4 year cycle)*/
    Year=GetNumber();
    Year=Year & 0x03;
    break;
   
   case 'E':              /* End Setup */
    WriteTime();
    done=1;
    break;

   default:               /* Error */
    putch('x');
    break;
   
  }
 }
 ShowConfig();
}
/************ END OF SetupTime ****************/


/****************************************************************
*                      GetTime
*
* Read the time from I2C, and convert into "hhmm\0" format
****************************************************************/
void GetTime(void)
{
 I2CBuffer[0]=0x00;                /* CMD Position */

 I2C_Send(I2CADDRESS,I2CBuffer,1); /* Tell to read from Start */

 I2C_Read(I2CADDRESS,I2CBuffer,7); /* Read CMD, Seconds,Minutes, hrs */

 
 Second=I2CBuffer[2];
 Minute=I2CBuffer[3];
 Hour=  I2CBuffer[4]&0x3F;   /* Clear out 12/24Hr mode bit and AM/PM bit */
 Year=  I2CBuffer[5] >> 6;   /* Move the Year to lower 2 bits */
 Date=  I2CBuffer[5]&0x3F;   /* Clear out the year */
 Month= I2CBuffer[6]&0x1F;   /* Clear out the weekday */
 
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
 I2CBuffer[5]=Hour & 0x7F; /* Ensure Hrs are in 24 Hour mode - clr bit 7*/
 I2CBuffer[6]= (Year << 6) | Date; /* Year and Date are combined */
 I2CBuffer[7]=Month;     /* Month */      
 
 I2C_Send(I2CADDRESS,I2CBuffer,8); /* Write Data to Clock Chip */
 

}
/************ END OF WriteTime ****************/



/****************************************************************
*                        Time2String
*
* Converts The BCD Time into a human readable string
****************************************************************/
void Time2String(void)
{
 char temp,d=0;


 
 /* Month */
 temp=Month&0xF0;        
 temp=temp>>4;
 CurrentTime[d++]=temp+'0'; 
 temp=Month&0x0F;
 CurrentTime[d++]=temp+'0';

 /* Date */
 temp=Date&0xF0;        
 temp=temp>>4;
 CurrentTime[d++]=temp+'0'; 
 temp=Date&0x0F;
 CurrentTime[d++]=temp+'0';

 /* Seperator Between Date and Time (For Spreadsheet import)*/
 CurrentTime[d++]=',';

 /* Hours */
 temp=Hour&0xF0;        
 temp=temp>>4;
 CurrentTime[d++]=temp+'0'; 
 temp=Hour&0x0F;
 CurrentTime[d++]=temp+'0';

 /* Break Between Hrs and Sec */
 CurrentTime[d++]=':';

 /* Minutes */
 temp=Minute&0xF0;
 temp=temp>>4;
 CurrentTime[d++]=temp+'0'; 
 temp=Minute&0x0F;
 CurrentTime[d++]=temp+'0';

 /* Break between Min and Sec */
 CurrentTime[d++]=':';
 
 
 /* Seconds */
 temp=Second&0xF0;
 temp=temp>>4;
 CurrentTime[d++]=temp+'0'; 
 temp=Second&0x0F;
 CurrentTime[d++]=temp+'0';

 /* Terminate the String */
 CurrentTime[d++]=0;  
 CurrentTime[15]=0;

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
 Limit=EEPROM_READ(LIMIT_ADDR); /* Read Limit from EEPROM */
}
/************ END OF GetLimit ****************/




/****************************************************************
*                       SetLimit
*
* Writes Limit to I2C Clock Chip
****************************************************************/
void SetLimit(unsigned char c)
{
 Limit=c & 0x0F;   /* Put lox 4 bits in */
 c = (c >>4) *10;  /* Convert High 4 bits to 10s num */
 Limit+=c;         /* Add 10s to limit */


 EEPROM_WRITE(LIMIT_ADDR,Limit); /* Write Limit to E2PROM */

}
/************ END OF  SetLimit ****************/

/****************************************************************
*                      PrintNumber
*
* Converts char number to string and prints it out
****************************************************************/
void PrintNumber(unsigned char num)
{
 unsigned char temp;

 if(num > 100)
 {
  temp=num/100;
  putch(temp+'0');
  temp=temp*100;
  num-=temp;
 }
 else
 {
  putch('0');
 }

 if(num >10)
 {
  temp=num/10;
  putch(temp+'0');
  temp=temp*10;
  num-=temp;
 }
 else
 {
  putch('0');
 }

 putch(num+'0');
 
}
/************ END OF  PrintNumber ****************/



/****************************************************************
*                      ShowConfig
*
*Displays version, time and setting
****************************************************************/
void ShowConfig(void)
{
 putch('\r');putch('\n');
	
 /* Get Limit from I2C */
 GetLimit();

 /* Print Version (Loaded in CurrentTime String) */
 puts(Version);
 putch('-');

 /* Get And Print the Current Time */
 GetTime(); 
 puts(CurrentTime);
 putch('-');

 /* Print the Current Limit Setting */
 PrintNumber(Limit);
 
 putch('\r');putch('\n');
}

/************ END OF ShowConfig ****************/
 
