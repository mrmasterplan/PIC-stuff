#define _16f690
#include "htc.h"

__CONFIG(WDTDIS & PWRTEN & MCLRDIS & BORDIS & UNPROTECT & INTIO);

unsigned char randchar(unsigned char);

main(){
	unsigned char i=0;
	unsigned char display=0b1111;
	TRISC=0;
	TRISA=0b100;
	OPTION=0b00000010;


	while(1){	
		
		i=randchar(255);
		while(--i!=0){
			while(!T0IF);
			T0IF=0;
		}
		if(RA3)
			display^=(0x1<<(randchar(255) & 0b11));
		else
			display=randchar(255)&0xF;
		PORTC=display;
	}
}

unsigned char randchar(unsigned char b){
	static unsigned char pre_x[]={50,200,0,174};
	static unsigned char n=0;
	static unsigned char c=0;
	unsigned int tmp0;

	n++;
	n%=4;

	tmp0=(577*(unsigned int)pre_x[n]+(unsigned int)c);


	c=tmp0/b;
	
	pre_x[n]=tmp0-c*b;

	return pre_x[n];
}