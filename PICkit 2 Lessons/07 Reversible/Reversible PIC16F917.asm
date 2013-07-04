;******************************************************************************
;Software License Agreement                                         
;                                                                    
;The software supplied herewith by Microchip Technology             
;Incorporated (the "Company") is intended and supplied to you, the  
;Company’s customer, for use solely and exclusively on Microchip    
;products. The software is owned by the Company and/or its supplier,
;and is protected under applicable copyright laws. All rights are   
;reserved. Any use in violation of the foregoing restrictions may   
;subject the user to criminal sanctions under applicable laws, as   
;well as to civil liability for the breach of the terms and         
;conditions of this license.                                        
;                                                                    
;THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,  
;WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED  
;TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A       
;PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,  
;IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR         
;CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.       
; *******************************************************************
; PICkit 2 Lesson 7 - Reversible LEDs
;
; This shows how to read the A2D converter and display the
; High order parts on the 4 bit LED display.
; The pot on the Low Pin Count Demo board varies the voltage 
; coming in on in A0.
;
; Also the switch is used to reverse the direction of rotation
;
; The A2D is referenced to the same Vdd as the device, which 
; is provided by the USB cable and nominally is 5V.  The A2D
; returns the ratio of the voltage on Pin RA0 to 5V.  The A2D
; has a resolution of 10 bits, with 1023 representing 5V and
; 0 representing 0V.

#include <p16F917.inc>
;	__CONFIG  _DEBUG_ON & _FCMEN_OFF &_IESO_OFF & _BOD_OFF & _CPD_OFF & _CP_OFF & _MCLRE_ON & _PWRTE_OFF & _WDT_OFF & _INTOSC ; _EXTRC ;   
	__CONFIG  _FCMEN_OFF &_IESO_OFF & _BOD_OFF & _CPD_OFF & _CP_OFF & _MCLRE_ON & _PWRTE_OFF & _WDT_OFF & _INTOSC ; _EXTRC ;   

	cblock	0x20
Delay1		; Assign an address to label Delay1
Delay2
Display		; define a variable to hold the diplay
Direction
LookingFor
	endc

	
	org 0
Start
	bsf	STATUS,RP0	; select Register Page 1
	movlw	0xFF
	movwf	TRISA		; Make PortA all input
	clrf	TRISD		; Make PortC all output
	movlw	0x10		; A2D Clock Fosc/8
	movwf	ADCON1
;	bcf	STATUS,RP0	; back to Register Page 0

	bcf	STATUS,RP0	; address Register Page 2
	bsf	STATUS,RP1	
	movlw	0x01		; configure Port A pin 0 Analog
	movwf	ANSEL
	bcf	STATUS,RP0	; address Register Page 0
	bcf	STATUS,RP1
	
	movlw	0x01
	movwf	ADCON0		; configure A2D for Channel 0 (RA0), Left justified, and turn on the A2D module
	movlw	0x08
	movwf	Display
	clrf	Direction
	clrf	LookingFor	; Looking for a 0 on the button
MainLoop
	movf	Display,w	; Copy the display to the LEDs
	movwf	PORTD
	nop			; wait 5uS for A2D amp to settle and capacitor to charge.
	nop			; wait 1uS
	nop			; wait 1uS
	nop			; wait 1uS
	nop			; wait 1uS
	bsf	ADCON0,GO_DONE	; start conversion
	btfsc	ADCON0,GO_DONE	; this bit will change to zero when the conversion is complete
	goto	$-1
	movf	ADRESH,w	; Copy the display to the LEDs
	movwf	Delay2

A2DDelayLoop
	incfsz	Delay1,f	; Waste time.  
	goto	A2DDelayLoop	; The Inner loop takes 3 instructions per loop * 256 loopss = 768 instructions
	incfsz	Delay2,f	; The outer loop takes and additional 3 instructions per lap * 256 loops
	goto	A2DDelayLoop	; (768+3) * 256 = 197376 instructions / 1M instructions per second = 0.197 sec.
				; call it a two-tenths of a second.

	movlw	.13		; Delay another 10mS plus whatever was above
	movwf	Delay2
TenmSdelay	
	decfsz	Delay1,f
	goto	TenmSdelay
	decfsz	Delay2,f
	goto	TenmSdelay

	btfsc	LookingFor,0
	goto	LookingFor1
LookingFor0
	btfsc	PORTB,0		; is the switch pressed (0)
	goto	Rotate
	bsf	LookingFor,0	; yes  Next we'll be looking for a 1
	movlw	0xFF		; load the W register incase we need it
	xorwf	Direction,f	; yes, flip the direction bit
	goto	Rotate

LookingFor1
	btfsc	PORTB,0		; is the switch pressed (0)
	bcf	LookingFor,0
	
Rotate
	bcf	STATUS,C	; ensure the carry bit is clear
	btfss	Direction,0
	goto	RotateLeft
RotateRight
	rrf	Display,f
	btfsc	STATUS,C	; Did the bit rotate into the carry?
	bsf	Display,7	; yes, put it into bit 3.

	goto	MainLoop
RotateLeft
	rlf	Display,f
	btfsc	STATUS,C	; did it rotate out of the display
	bsf	Display,0	; yes, put it into bit 0
	goto	MainLoop
	
	end
	
