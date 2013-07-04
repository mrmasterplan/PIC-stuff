/*
 *	Delay functions for HI-TECH C on the PIC
 *
 *	Functions available:
 *		DelayUs(x)	Delay specified number of microseconds
 *		DelayMs(x)	Delay specified number of milliseconds
 *
 *	Note that there are range limits: x must not exceed 255 - for xtal
 *	frequencies > 12MHz the range for DelayUs is even smaller.
 *	To use DelayUs it is only necessary to include this file; to use
 *	DelayMs you must include delay.c in your project.
 *
 */

/*	Set the crystal frequency in the CPP predefined symbols list in
	HPDPIC, or on the PICC commmand line, e.g.
	picc -DXTAL_FREQ=4MHZ
	
	or
	picc -DXTAL_FREQ=100KHZ
	
	Note that this is the crystal frequency, the CPU clock is
	divided by 4.

 *	MAKE SURE this code is compiled with full optimization!!!
	
 */

#ifndef DELAY_ROUTINE       //-- Check if already loaded!!!

 #define DELAY_ROUTINE      //-- MAKE SURE CANNOT RELOAD
 #ifndef	XTAL_FREQ
  #error NO XTAL_FREQ DEFINED - Is Required for DelayUs!
  #define	XTAL_FREQ	8MHZ		/* Crystal frequency in MHz */
 #endif

 #define	MHZ	*1000			/* number of kHz in a MHz */
 #define	KHZ	*1			/* number of kHz in a kHz */

 #if	XTAL_FREQ >= 12MHZ
  #if XTAL_FREQ >= 20MHZ
   //---- For Clocks faster than 20MHz
   #define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)*((XTAL_FREQ)/(24MHZ)); \
			  while(--_dcnt != 0); \
			  _dcnt = (x)*((XTAL_FREQ)/(24MHZ)); \
			  while(--_dcnt != 0) \
			  continue; }
   #else
   //----- For clocks 12 MHz and faster
   #define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)*((XTAL_FREQ)/(12MHZ)); \
			  while(--_dcnt != 0) \
				  continue; }
  #endif
 #else
  //----- For Clocks less than 12MHz
  #define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)/(12MHZ/(XTAL_FREQ))|1; \
			  while(--_dcnt != 0) \
				  continue; }
 #endif

 extern void DelayMs(unsigned char);
 extern void Pause(unsigned char);       //-- Approx delay in seconds
#endif


