#define _16f690
#include "htc.h"
//#include "math.h"
//#define  _AN0  0b10000001
//#define  _AN1  0b10000101
//#define  _AN2  0b10001001
//#define _paste(a,b)     a##b
//#define bitof(var,num)  (((bitv *)&(var))->_paste(b,num))
//#define x4      bitof(myvar, 4)   // example             /// read about unions!!!

__CONFIG(0x0044); //external HS oscillator & Power up timer

//void A2Daccq(char*,char);
//void _error(void);

// structures and unions decalrations
/*struct tmr2struct {
		unsigned t2ckps0 	: 1;
		unsigned t2ckps1 	: 1;
		unsigned tmr2on 	: 1;
		unsigned toutps 	: 4;
		unsigned unused0 	: 1;
		unsigned pr2		: 8;
	} ;

struct tmr2outst {
	char T2_con;
	char PR2;
	};

union tmr2conuni {
	struct tmr2struct bitwise;
	struct tmr2outst  bytewise;
	};	*/

/*struct bittest {
	unsigned zero :1;
	unsigned one :1;
	unsigned two :1;
	unsigned three :1;
	unsigned four :1;
	unsigned five :1;
	unsigned six :1;
	unsigned seven :1;
	};*/

// global variable declarations
//bank1 	unsigned int base, fact;
//	union tmr2conuni T2_Arr[10] ;
//	unsigned int cycles[10], cyccount;
//	unsigned int period;
//bank1	unsigned long speed;    
//		struct bittest *bitptr;
//		float factor, basis, fleriod;
//		char  i, j,t2ps, inti,intii;
//	char i, ref;



void main(void){


	// configure ports
	TRISA=0;
//	TRISB=0;
	TRISC=0;
	PORTC=0b00000010;
	PORTA=2;

	
	PEIE=1;
	TMR2IE=1;
	TMR2IF=0;
	T2CON=255;
	PR2=255;
	GIE=1;

	
//117	61	30	30	53	86	101	37	124	108
//139	236	107	97	201	29	89	210	238	246

/*
	T2_Arr[0].bytewise.T2_con=117;
	T2_Arr[0].bytewise.PR2=139;
	cycles[0]=24;

	T2_Arr[1].bytewise.T2_con=61;
	T2_Arr[1].bytewise.PR2=236;
	cycles[1]=26;

	T2_Arr[2].bytewise.T2_con=30;
	T2_Arr[2].bytewise.PR2=107;
	cycles[2]=29;

	T2_Arr[3].bytewise.T2_con=30;
	T2_Arr[3].bytewise.PR2=97;
	cycles[3]=32;

	T2_Arr[4].bytewise.T2_con=53;
	T2_Arr[4].bytewise.PR2=201;
	cycles[4]=35;

	T2_Arr[5].bytewise.T2_con=86;
	T2_Arr[5].bytewise.PR2=29;
	cycles[5]=39;

	T2_Arr[6].bytewise.T2_con=101;
	T2_Arr[6].bytewise.PR2=89;
	cycles[6]=43;

	T2_Arr[7].bytewise.T2_con=37;
	T2_Arr[7].bytewise.PR2=210;
	cycles[7]=47;

	T2_Arr[8].bytewise.T2_con=124;
	T2_Arr[8].bytewise.PR2=238;
	cycles[8]=52;

	T2_Arr[9].bytewise.T2_con=108;
	T2_Arr[9].bytewise.PR2=246;
	cycles[9]=57; */

	while(1){
	/*	while(TMR2IF==0);
		PORTA^=0b00000110;
		if(PORTC==1) PORTC=2;
		else PORTC=1;
		TMR2IF=0;
		PORTC&=4;*/
		}
	}

interrupt service(void){
	GIE=0;
//	cyccount--;
//	if(cyccount==0){
//		i++;
//		if(i>18)i=0;
//		if(i>9) ref=19-i;
//		else ref=i;
//		T2CON=T2_Arr[ref].bytewise.T2_con;
//		PR2=T2_Arr[ref].bytewise.PR2;
//		cyccount=cycles[ref];
//		}
//	PORTA^=0b00000110;
	if(PORTC==1) PORTC=2;
	else PORTC=1;
	TMR2IF=0;
//	T2CON=255;
//	PR2=255;
	GIE=1;
	}



//void A2Daccq(char *_result,char _channel){
//
//	ADCON0 = _channel;		// set the right channel bits
//	GODONE=1;							// lets rock;
//	while(GODONE);					// wait for A2D to finish
//	*_result=ADRESL;
//	_result++;
//	*_result=ADRESH;
//}


//void _error(void){
//	PORTC=0b01010101;
//	while(1);
//	}

