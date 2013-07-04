

	

SEND	macro	S_string	; "SEND 'X'" sends character to RS232
	movlw	S_string
	BANK0
	call	SENDsub
	endm


;***** SUBROUTINES *****

SENDsub	
	btfss	TXIF
	goto	SENDsub
	movwf	TXREG

	RETURN

