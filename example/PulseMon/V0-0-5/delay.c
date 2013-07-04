/*
 *	Delay functions
 *	See delay.h for details
 *
 *	Make sure this code is compiled with full optimization!!!
 */

 /* Added WDTCLR() instructions 16 March 1999  M.O.Pearce ---- */

 /* Version 0.1.0 */

#include	"delay.h"

void
DelayMs(unsigned char cnt)
{
#if	XTAL_FREQ <= 2MHZ
	do
 {
  CLRWDT();
		DelayUs(995);
	} while(--cnt);
#endif

#if XTAL_FREQ > 2MHZ
 #if XTAL_FREQ > 8MHZ
	 unsigned char	i;
	 do
  {
	 	i = 8;
   CLRWDT();
	 	do
   {
	 		DelayUs(123);
 		} while(--i);
 	} while(--cnt);
 #else
	 unsigned char	i;
	 do
  {
   CLRWDT();
	 	i = 4;
	 	do
   {
	 		DelayUs(250);
 		} while(--i);
 	} while(--cnt);
 #endif
#endif
}

//************* Pause - added 16 March  1999 M.O.Pearce *************
// Causes delay in seconds
void Pause (unsigned char Secs)
{
 char count;
 do
 {
  for(count=0;count<4;count++)
  {
   DelayMs(250);
  }
 }while(--Secs >0);
}



