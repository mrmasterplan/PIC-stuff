#define _16f690
#include "htc.h"
#include "math.h"
#define  _AN0  0b10000001
#define  _AN1  0b10000101
#define  _AN2  0b10001001
#define _paste(a,b)     a##b
#define bitof(var,num)  (((bitv *)&(var))->_paste(b,num))
//#define x4      bitof(myvar, 4)   // example             /// read about unions!!!

__CONFIG(0x0044); //external HS oscillator & Power up timer

void A2Daccq(char*,char);
//void _error(void);

// structures and unions decalrations
struct tmr2struct {
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
	};	

struct bittest {
	unsigned zero :1;
	unsigned one :1;
	unsigned two :1;
	unsigned three :1;
	unsigned four :1;
	unsigned five :1;
	unsigned six :1;
	unsigned seven :1;
	};

// global variable declarations
bank1 	unsigned int base, fact;
bank1	union tmr2conuni T2_Arr[10] ;
bank1	unsigned int period, cycles[10], cyccount;
bank1	unsigned long speed;    
		struct bittest *bitptr;
		float factor, basis, fleriod;
		char  i, j,t2ps, inti,intii;

interrupt isr(void){
	GIE=0;
	if(!cyccount--){
		if((inti++)>18)inti=0;
		if(inti ==1){
				PORTB&=192;
				PORTC=1;
				}
		else if(inti<8) PORTC<<=1;
		else if(inti==8){
				PORTC=0;
				PORTB|=16;
				}
		else if(inti<11) PORTB^=48;
		else if(inti==11){
				PORTB&=192;
				PORTC=128;
				}
		else	PORTC>>=1;

		if(inti<10) intii = inti;
		else intii=18-inti;
		cyccount = cycles[intii];
		T2CON=T2_Arr[intii].bytewise.T2_con;
		PR2=T2_Arr[intii].bytewise.PR2;
		}
	PORTB^=192;
	TMR2IF=0;
	GIE=1;
	}


void main(void){

	//variables
//	char*	charp;

	// configure ports
	TRISA=0b1110000;
	TRISB=0;
	TRISC=0;
	PORTB=128;

	//A2D init
	ANSEL=0b00000111;	// A0,1,2 as analog
	ANSELH=0;
	ADCON0=0b10000001; 	// right justified & turn on A2D module
	ADCON1=0b00100000;	// clock =Fosc/32
	ADIE=0;
	
	PEIE=1;
	TMR2IE=1;
	TMR2IF=0;
	GIE=1;
	PR2=255;
	T2CON=255;

//117	61	30	30	53	86	101	37	125	109
//139	236	107	97	201	29	89	210	238	246


	T2_Arr[0].bytewise.T2_con=117;
	T2_Arr[0].bytewise.PR2=139;
	T2_Arr[1].bytewise.T2_con=61;
	T2_Arr[1].bytewise.PR2=236;
	T2_Arr[2].bytewise.T2_con=30;
	T2_Arr[2].bytewise.PR2=107;
	T2_Arr[3].bytewise.T2_con=30;
	T2_Arr[3].bytewise.PR2=97;
	T2_Arr[4].bytewise.T2_con=53;
	T2_Arr[4].bytewise.PR2=201;
	T2_Arr[5].bytewise.T2_con=86;
	T2_Arr[5].bytewise.PR2=29;
	T2_Arr[6].bytewise.T2_con=101;
	T2_Arr[6].bytewise.PR2=89;
	T2_Arr[7].bytewise.T2_con=37;
	T2_Arr[7].bytewise.PR2=210;
	T2_Arr[8].bytewise.T2_con=124;
	T2_Arr[8].bytewise.PR2=238;
	T2_Arr[9].bytewise.T2_con=108;
	T2_Arr[9].bytewise.PR2=246;

	while(1){ /*
	
		A2Daccq(&base,_AN2);
		A2Daccq(&fact,_AN0);		// let A2Daccq test the channel and thus decide where to write the result
		A2Daccq(&speed,_AN1);


	

	// scale base and fact. max pariod = 25000, min period = 333
	// for easy averaging take the high byte.
		base>>=2;
		fact>>=2;
		speed>>=2;
	
		// base is 200 to 2000Hz
		basis = 200+ (unsigned int)((float)base*7.03125);
		factor=25000.0 /basis;
		factor =(unsigned int) (((float) (fact/256)) * exp(log((double)factor ))/10.0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		speed =	131600 + (1540*speed);
	
	
		//set prescaler
		//see if period is divisible by 4 or 16 if yes, great, take care 
		//of that by the prescaler and pass on the remaining value. If not 
		//but it's still bigger than the factimum value
		//reachable by 255*16 then decrement it and see if it is divisible again.
		//repeat until the result you end up with fits into the frame. 
		for(i=0;i<10;i++){
			fleriod=basis;
			for(j=0;j<i;j++)	fleriod =fleriod*factor; 
				
			period=(unsigned int)fleriod;
			cycles[i]=(unsigned int)(speed/period);
			
			bitptr=&period;
			while(1){		
				if((*bitptr).zero & (*bitptr).one) {			//if priod can be divided by 4
					T2_Arr[i].bitwise.t2ckps0=1;										//we can take care of that with a prescaler
					period>>=2;
					if((*bitptr).zero & (*bitptr).one) {							//if it can be divided by 4 again,
						T2_Arr[i].bitwise.t2ckps1=1;									//even better
						period>>=2;
						}
					}
				if(period<=4080) break;			
				else period--; 
				}
			// now the corresponding prescaler has been set
			// we need to find the postscaler = 1..16 inclusive
			t2ps=0;
	//		if(!(*bitptr).zero){
	//			t2ps|=2;
	//			period>>=1;
	//			if(!(*bitptr).one){
	//				t2ps|=4;
	//				period>>=1;
	//				if(!(*bitptr).two){
	//					t2ps|=8;
	//					period>>=1;
	//					if(!(*bitptr).three){
	//						t2ps|=16;
	//						period>>=1;
	//						}
	//					}
	//				}
	//			}
			for(j=16;j>=1;j--){
				if(period>(255*i)){
					period=period/i;
					t2ps=t2ps*i;
					break;
					}
				}
				
//			if(period>255) _error();		
//			if(i>16) _error();
			// finished determining, dec by one (because of def of PS) and put into the structure
			t2ps--;
			T2_Arr[i].bitwise.toutps=t2ps;
			T2_Arr[i].bitwise.pr2=period;
			T2_Arr[i].bitwise.tmr2on=1;
			
			}	*/								
		}
}
 

void A2Daccq(char *_result,char _channel){

	ADCON0 = _channel;		// set the right channel bits
	GODONE=1;							// lets rock;
	while(GODONE);					// wait for A2D to finish
	*_result=ADRESL;
	_result++;
	*_result=ADRESH;
}


//void _error(void){
//	PORTC=0b01010101;
//	while(1);
//	}

