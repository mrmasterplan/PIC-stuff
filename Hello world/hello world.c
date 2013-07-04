#include <htc.h>
	unsigned int i;
	unsigned int j;
void main(void){
	__CONFIG(0xD4);
	TRISC=0;
	TRISA=0;
	while(1){
		PORTC=1;
		for(i=0;i<=10000;i++);
		PORTC=2;
		for(i=0;i<=10000;i++);
		PORTC=4;
		for(i=0;i<=10000;i++);
		PORTC=8;
		for(i=0;i<=10000;i++);
		for(i=0;i<=200;i++){
			PORTA=1;
			for(j=0;j<=150;j++);
			PORTA=2;
			for(j=0;j<=150;j++);
		}
	}
}
