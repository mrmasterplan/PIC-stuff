
#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock	0x20
Delay1			; Assign an address to label Delay1
Delay2	
Sendnext
Justsent
	endc
	
; Flag Definitions

	cblock 0x70	; put these up in unbanked RAM
Spbrgcount
	endc
	
	org 0
Start

;
;	BAUDCTL,SCKP invert trans. pin if set
;
;	PIR1,TXIF Set? ->Clear to transmit ;0
; Load TXREG

;	PIR1,RCIF when ready to read   ;0
;	Read RCREG   ;0
;	RCSTA,OERR Set? -> Clear CREN and set again,   ;0
;
	bsf	STATUS,RP0	; select Register Page 1
	movlw	0xFF
	movwf	TRISA		; Make PortA all input
	clrf	TRISC		; Make PortC all output

	bcf	TXSTA,SYNC		;1
	bcf TXSTA,BRGH		;1
	bcf BAUDCTL,BRG16	;1

;	SPBRG->Count down
	movlw 0xFF
	movwf SPBRG			;1
	movwf Spbrgcount

	bsf	TXSTA,TXEN		;1
	bcf	STATUS,RP0	; address Register Page 0
	bcf	STATUS,RP1

	bsf RCSTA,CREN		 ;0
	bsf	RCSTA,SPEN		;0

	clrf ADCON0    ;0

MainLoop

	clrf Sendnext

SendLoop
	btfss PIR1,TXIF		; Ready to send?
	goto SendLoop
	
	movf Sendnext,w
	movwf TXREG
	movwf Justsent

	bsf PORTA,0
	call Delay
	call Delay
	bcf PORTA,0
	call Delay

ReadBreak1
	btfss PIR1,RCIF   ; Wait for a charachter to be received
 	goto ReadBreak1
	
	bcf STATUS,Z
	movf RCREG,w

	subwf Justsent,f
	
	btfss STATUS,Z
	goto Transmiterror	

	goto Success

	decfsz Sendnext,f
	goto SendLoop	

; make the clock faster and repeat

;	bsf	STATUS,RP0	; select Register Page 1
;	movf Spbrgcount,w
;	movwf SPBRG
;	bcf STATUS,RP0	; select Register Page 0
	
	decfsz Spbrgcount,f
	goto MainLoop

	;Flash the Diode on A1
Success
	bsf PORTA,0
	call Delay
	bcf PORTA,0
	call Delay
	goto Success


Transmiterror
	;turn on the diode on A1
	bsf PORTA,0
	goto Transmiterror




; Delay Subroutine.  Enter delays Wreg * 771uS + 5 uS including call and return
Delay	movwf	Delay2		;
DelayLoop
	decfsz	Delay1,f	; Waste time.  
	goto	DelayLoop	; The Inner loop takes 3 instructions per loop * 256 loopss = 768 instructions
	decfsz	Delay2,f	; The outer loop takes and additional 3 instructions per lap * 256 loops
	goto	DelayLoop	; (768+3) * 256 = 197376 instructions / 1M instructions per second = 0.197 sec.
				; call it a two-tenths of a second.
	return
	
	end
	
