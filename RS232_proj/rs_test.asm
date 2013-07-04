
;***** PROCESSOR DECLARATION & CONFIGURATION *****

	#include "p16f690.inc"
	
	; embed Configuration Data within .asm File.
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _BOR_OFF & _CP_OFF & _IESO_OFF & _FCMEN_OFF)

;***** MEMORY STRUCTURE *****

;***** REGISTER DECLARATION *****
	cblock 0x20
;FLAGreg
;TXD
;RXD
;W_TEMP
;STATUS_TEMP
;PCLATH_TEMP
;FSR_TEMP
Send_count
Wait_count
Wait_count2
temp
	endc
	
;	cblock 0x70
;W_Save
;STATUS_Save
;FLAGS_Save
;	endc
	

;	LF	equ	d'10'	; Line Feed
;	CR	equ	d'13'	; Carriage Return
;	TAB equ d'9'	; Tabulator
;	BS	equ	d'8'	; Backspace


	ORG     0x00			; processor reset vector
  	goto    MAIN

;	ORG     0x04			; interrupt vector
;	goto	ISR			; Interrupt Service Routine (ISR)


BANK0	macro			; select register bank 0
	bcf     STATUS,RP0
	bcf     STATUS,RP1
	endm
	
BANK1	macro			; select register bank 1
	bsf     STATUS,RP0
	bcf     STATUS,RP1
	endm

BANK2	macro			; select register bank 2
	bcf     STATUS,RP0
	bsf     STATUS,RP1
	endm

BANK3	macro			; select register bank 3
	bsf     STATUS,RP0
	bsf     STATUS,RP1
	endm

;************** MAIN **************

MAIN	
	bcf	INTCON,GIE			; no interrupts at all
	BANK1
	clrf TRISC
	bsf TRISB,7
;	BANK0
	
;-----initialise EUSART
;	bcf		BAUDCTL,BRG16
;	movlw	0x19
;	movwf	SPBRG       ; BRG16/BRGH/SYNC=0/1/0 + SPBRG=47 gives 19.2kb @ 8MHz fOsc
;	bcf	TXSTA,SYNC		; asynchonous
;	bsf	TXSTA, BRGH
;	BANK0
;	bsf	RCSTA,SPEN		; set function of TX pin
;	BANK1
;	bcf	TXSTA,TX9			; set 8 bit communication
;	bsf	PIE1,TXIE		; no send interrupts
;	bsf	PIE1,RCIE		; receiver interrupts DISABLED
;	BANK0
;	bcf RCSTA,RX9			; 8 bit com.
;	BANK1	
;	bsf	TXSTA,TXEN		; enable transmitter
;	BANK0
;	bsf RCSTA,CREN		; enable receiver
;	bsf INTCON,PEIE	
;	bcf PIR1,RCIF
;-------End intialize EUSART


	bsf PORTC,0
	movlw 0xAA
	movwf Send_count
	movlw b'00000100'
	movwf T2CON
	BANK1
	movlw 0x68
	movwf PR2
	BANK0
	bcf PIR1,TMR2IF
	bsf PORTB,7
LOOP
;	btfss PIR1, RCIF
;	goto LOOP
;	bcf	PIR1, RCIF
;	movf RCREG,w
;	movwf PORTC
	movf Send_count,w
	call SendRoutine

;	movwf TXREG
	movwf PORTC
	decf Send_count,f
	call Wait
;	btfss RCSTA, OERR
;	goto LOOP
;	bcf RCSTA,CREN		; disable receiver
 ;	bsf RCSTA,CREN		; enable receiver, clears OERR

	

	goto	LOOP

Wait
	decfsz Wait_count,f
	goto Wait
	decfsz Wait_count2,f
	goto Wait
	return

SendRoutine
	movwf temp
	call WaitBlip
	bcf PORTB,7
	call WaitBlip
	movwf PORTB
	;--------------------------unfinished	
	

WaitBlip
	bcf PIR1,TMR2IF
	btfss PIR1,TMR2IF
	goto$-1
	return


end
