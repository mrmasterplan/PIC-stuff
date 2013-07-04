
#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock 0x20
count
;display
temp
	endc

	org 0	
Start
	bsf	STATUS,RP0	; select Register Page 1
	clrf	TRISC		; Make PortC all output
	bcf	STATUS,RP0	; back to Register Page 0
	movlw b'01001101'
	movwf T2CON       ; select 1:4 prescaler & T2 ON & 1:10 Postscaler
	
	bsf	STATUS,RP0	; select Register Page 1	
	movlw 0x32 		  ;set PR2 to 125     125 * 4 * 10 = 5000
	movwf PR2		  ;---> should give ~5mm per pixel.
	bcf	STATUS,RP0	; back to Register Page 0
	
	clrf count
	movf count,w
	call GetNewLine  ;load the first line into the memory

MainLoop
	btfss	PIR1,TMR2IF	; wait until the next pixel is to be displayed
	goto	$-1
	
;	movwf	display		; for debuggig purposes
	movwf   PORTC    ; put the new line into the display
	
	bcf		PIR1,TMR2IF	; clear the interrupt flag

	incf	count,f		;increment the line counter
	
	bcf	STATUS,Z		; compare if count>58
	movlw	0x3B
	subwf	count,w
	btfsc	STATUS,Z	
	clrf	count		; if yes, reset it.

	movf	count,w
	call	GetNewLine ; load new line into accumulator

	goto	MainLoop
	
		
; Get counter, return 8 bit pattern of next display line.
; 
	org	0xf7	; force table to cross a page boundary
GetNewLine
	movwf	temp
	movlw	high TableStart	; get high order part of the beginning of the table
	movwf	PCLATH
	movlw	low TableStart	; load starting address of table
	bcf		STATUS,C
	addwf	temp,w		; add offset
	btfsc	STATUS,C	; did it overflow?
	incf	PCLATH,f	; yes: increment PCLATH
	movwf	PCL		; modify PCL
TableStart 			; 60 entries
	retlw	b'10000001'	; H
	retlw	b'11111111'	; 
	retlw	b'00001000'	; 
	retlw	b'00001000'	; 
	retlw	b'00001000'	; 
	retlw	b'11111111'	; 
	retlw	b'10000001'	; /H
	retlw	b'01111000'	; e
	retlw	b'10010100'	; 
	retlw	b'10010100' ; 
	retlw	b'10010100' ;
	retlw	b'01011000' ; /e
	retlw	b'00000000' ; 
	retlw	b'11111111' ; l
	retlw	b'10000000' ;/l
	retlw	b'11111111' ; l
	retlw	b'10000000' ;/l
	retlw	b'01111000'	; o
	retlw	b'10000100'	; 
	retlw	b'10000100' ; 
	retlw	b'10000100' ;
	retlw	b'01111000' ; /o
	retlw	b'00000000' ; 
	retlw	b'00000001' ; W
	retlw	b'00000011' ; 
	retlw	b'00111100' ; 
	retlw	b'11000000' ; 
	retlw	b'00111100' ; 
	retlw	b'00000010' ; 
	retlw	b'00111100' ; 
	retlw	b'11000000' ; 
	retlw	b'00111100' ; 
	retlw	b'00000011' ; 
	retlw	b'01111001' ; /W o
	retlw	b'10000100'	; 
	retlw	b'10000100' ; 
	retlw	b'10000100' ;
	retlw	b'01111000' ; /o
	retlw	b'10000000' ; r
	retlw	b'11111100' ; 
	retlw	b'10001000' ; 
	retlw	b'00000100' ; 
	retlw	b'00000000' ; 
	retlw	b'11111111' ; l
	retlw	b'10000000' ;/l
	retlw	b'01111000' ;d
	retlw	b'10000100' ; 
	retlw	b'10000100' ; 
	retlw	b'11111111' ; 
	retlw	b'10000001' ; /d
	retlw	b'00000000' ; 
	retlw	b'00000000' ;
	retlw	b'01000111' ;! 
	retlw	b'10101101' ; 
	retlw	b'01000111' ; /! 
	retlw	b'00000000' ;
	retlw	b'00000000' ;
	retlw	b'00000000' ;
	retlw	b'00000000' ;
	retlw	b'00000000' ;
	end
