
#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock	0x20
Delay1			; Assign an address to label Delay1
Delay2	
Display 		; define a variable to hold the diplay
Direction 
LookingFor
T0Semaphore
	endc
	
; Flag Definitions

	cblock 0x70	; put these up in unbanked RAM
W_Save
STATUS_Save
	endc

	org 0
	goto	Start
	nop
	nop
	nop
ISR	movwf	W_Save
	movf	STATUS,w
	movwf	STATUS_Save
	
;	btfsc	PIR1,T1IF	; Uncomment if you needed to check Timer 1
;	goto	ServiceTimer1
	btfsc	INTCON,T0IF
	goto	ServiceTimer0
;	btfsc	PIC1,ADIF	; Uncomment if you need to check the ADC
;	goto	ServiceADC
	goto	ExitISR		
	
ServiceTimer0
	bcf	INTCON,T0IF	; clear the interrupt flag. (must be done in software)
	bsf	T0Semaphore,0	; signal the main routine that the Timer has expired
	bsf	ADCON0,GO	; start conversion
	btfsc	ADCON0,GO	; this bit will change to zero when the conversion is complete
	goto	$-1
	comf	ADRESH,w	; Form the 1's complement of ADresult
	movwf	TMR0		; Also clears the prescaler
	goto	ExitISR
			
ExitISR
	movf	STATUS_Save,w
	movwf	STATUS
	swapf	W_Save,f
	swapf	W_Save,w
	retfie
	
;
;TMR0:
;  OPTION_REG,
;    T0CS=0
;	 PSA=0
;    PS2=0
;    PS1=0
;    PS0=1
;  INTCON
;    GIE=1
;    T0IE=0
;    T0IF=0

	
Start
	bsf	STATUS,RP0	; select Register Page 1
	movlw	0xFF
	movwf	TRISA		; Make PortA all input
	clrf	TRISC		; Make PortC all output

;	movlw	0x10		; A2D Clock Fosc/8
;	movwf	ADCON1

	movlw	B'10000001'	; configure Prescaler on Timer0, max prescale (/256)
	movwf	OPTION_REG	; configure
;	bcf	STATUS,RP0	; back to Register Page 0

;	bcf	STATUS,RP0	; address Register Page 2
;	bsf	STATUS,RP1	
;	movlw	0x01		; we want all Port A pins Analog, except RA3
;	movwf	ANSEL
;	bcf	STATUS,RP0	; address Register Page 0
;	bcf	STATUS,RP1
	
;	movlw	0x01
;	movwf	ADCON0		; configure A2D for Channel 0 (RA0), Left justified, and turn on the A2D module
;	movlw	0x08
;	movwf	Display
;	clrf	Direction
;	clrf	LookingFor	; Looking for a 0 on the button
	
	
	movlw	B'10000000'	; enable Timer 0 and global interrupts
	movwf	INTCON
MainLoop
	btfss	T0Semaphore,0	; did the Timer0 overflow?
	goto	CheckButton	; no - go monitor the button
	bcf	T0Semaphore,0	; clear the flag	
	movf	Display,w	; Copy the display to the LEDs
	movwf	PORTC
	
Rotate
	bcf	STATUS,C	; ensure the carry bit is clear
	btfss	Direction,0
	goto	RotateLeft
RotateRight
	rrf	Display,f
	btfsc	STATUS,C	; Did the bit rotate into the carry?
	bsf	Display,3	; yes, put it into bit 3.

	goto	CheckButton
RotateLeft
	rlf	Display,f	; rotate in place
	btfsc	Display,4	; did it rotate out of the display
	bsf	Display,0	; yes, put it into bit 0

CheckButton
	btfsc	LookingFor,0	; which direction are we looking for
	goto	LookingFor1
LookingFor0
	btfsc	PORTA,3		; is the switch pressed (0)
	goto	EndMainLoop
	bsf	LookingFor,0	; yes  Next we'll be looking for a 1
	movlw	0xFF		; load the W register incase we need it
	xorwf	Direction,f	; yes, flip the direction bit
	goto	EndMainLoop

LookingFor1
	btfsc	PORTA,3		; is the switch pressed (0)
	bcf	LookingFor,0

EndMainLoop
	movlw	.13
	call	Delay		; delay ~10mS (13 * 775uS)
	goto	MainLoop

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
	
