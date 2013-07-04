//-----------------------------------------------------------------
//                       mikeslib.c
//
// Mikes header file with handy definitions and typedefs in it
//
//-----------------------------------------------------------------

#ifndef MikesLibrary
 #define MikesLibrary

 //#include	<conio.h>

 //--- Short cuts for unsigned int and unsigned char ---
 #ifndef uint
  typedef unsigned int  uint;
 #endif
 #ifndef uchar
  typedef unsigned char uchar;
 #endif
 #ifndef ulong
  typedef unsigned long ulong;
 #endif

 #ifndef BITS
  //--- Structure defining 8 individual bits ---
  typedef struct
  {
	  uint a : 1;
	  uint b : 1;
	  uint c : 1;
	  uint d : 1;
	  uint e : 1;
	  uint f : 1;
	  uint g : 1;
	  uint h : 1;
  }BITS;
 #endif

 //--- Union for converting a BYTE to individual Bits ---
 #ifndef BITBYTE
  typedef union
  {
  	uchar B;
  	BITS  b;
  }BITBYTE;
 #endif

 //--- For defining port bits ------
 #ifndef PORTBIT
  #define PORTBIT(adr, bit)       ((unsigned)(&adr)*8+(bit))
 #endif

 //*************************************************************************
 // Following is the Software Driven Serial driver written but HI-TECH
 //
 // To Use this routine use
 //       #define SERIALSOFTWARE
 //
 // For Interrupt driven routine (not yet avaliable)
 //       #define SERIALINTERRUPT
 //
 // For Hardware driven routines (Not yet avaliable)
 //       #define SERIALHARDWARE
 //
 //
 //  Here is an example setup for the serial
 // #define	XTAL   20000000        // Crystal frequency in Hz
 // #define SERIALSOFTWARE         // Indicate type of serial output
 // #define TxPort PORTB           // Pin on Port B
 // #define RxPort PORTB
 // #define TxBit  2               // Pin Number to use
 // #define RxBit  1
 // #define TxTris TRISB           // Tris Register for Pin
 // #define RxTris TRISB
 // #define BRATE 9600             // Baud rate required
 //
 //*************************************************************************
 #ifdef SERIALSOFTWARE
  /*
  *	Serial port driver for 16Cxx chips
  *	using software delays.
  *
  *	Copyright (C)1996 HI-TECH Software.
  *	Freely distubutable.
  */

  /*
   *	Tunable parameters
  */
  /*	Transmit and Receive port bits */
  #ifndef TxPort
   #define TxTris TRISA
   #define TxPort PORTA    //-- PORT A Bit 1 for Tx
   #define TxBit  1
  #endif

  #ifndef RxPort
   #define RxTris TRISA
   #define RxPort PORTA    //-- PORT A Bit 0 for Rx
   #define RxBit  0
  #endif

  /*	Xtal frequency */


  #ifndef XTAL
   #define	XTAL	4000000
  #endif

  /*	Baud rate	*/
  #ifndef BRATE
   #define	BRATE	9600
  #endif

  /*	Don't change anything else */

  #define	DLY		3		/* cycles per null loop */
  #define	TX_OHEAD	13		/* overhead cycles per loop */
  #define	RX_OHEAD	12		/* receiver overhead per loop */

  #define	RSDELAY(ohead)	(((XTAL/4/BRATE)-(ohead))/DLY)

  //******************************************************************
  // Serial Initialisation Routine
  // Michael O. Pearce
  // Sets up the ports etc for the following routines
  //******************************************************************
  static bit   TxData @ PORTBIT(TxPort,TxBit);   //-- TXD Pin
  static bit   RxData @ PORTBIT(RxPort,RxBit);   //-- RXD Pin
  static bank1 bit TxTRIS @ PORTBIT(TxTris,TxBit);
  static bank1 bit RxTRIS @ PORTBIT(RxTris,RxBit);
  bit   TxRxInit = 0;
  void InitSerial (void)
  {

   TxData=1;          //-- set pin high to start with

   TxTRIS = 0;
   RxTRIS = 1;

   TxRxInit=1;
  }
  //******************************************************************
  //  Software Putch Routine
  //  HI-TECH
  //  Requires definition of TxData for output pin
  //******************************************************************
  void putch(char c)
  {
   unsigned char dly, bitno;
   bitno = 11;

   if(TxRxInit==0)
   {
    InitSerial();
   }

   TxData = 0;			/* start bit */
   bitno = 12;
   do
   {
    dly = RSDELAY(TX_OHEAD);	/* wait one bit time */
    do
    {
     /* nix */ ;
    }while(--dly);
    if(c & 1) TxData = 1;
    if(!(c & 1)) TxData = 0;
    c = (c >> 1) | 0x80;
   } while(--bitno);
  }

  //******************************************************************
  //  Software Getch Routine
  //  HI-TECH
  //  Requires Definition of RxData for port pin
  //******************************************************************
  char getch(void)
  {
   unsigned char c, bitno, dly;
   if(TxRxInit==0)
   {
    InitSerial();
   }
   for(;;)
   {
    while(RxData) continue;	/* wait for start bit */
    dly = RSDELAY(3)/2;
    do/* nix */;
    while(--dly);
    if(RxData) continue;	/* twas just noise */
    bitno = 8;
    c = 0;
    do
    {
     dly = RSDELAY(RX_OHEAD);
     do
     /* nix */;
     while(--dly);
     c = (c >> 1) | (RxData << 7);
    } while(--bitno);
    return c;
   }
  }
 #endif               //-- End of Serial Software routines
 //--------------------------------------------------------------------------
#endif
