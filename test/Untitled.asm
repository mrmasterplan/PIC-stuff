
#include <p16F690.inc>

__config(0x42)

	bsf	STATUS,RP0	; select Register Page 1
	bcf	TRISC,0		; make IO Pin C0 an output
	bcf	STATUS,RP0	; back to Register Page 0
	bsf	PORTC,0		; turn on LED C0 (DS1)
	goto	$		; wait here
	end
