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
; PICkit 2 Lesson 5 - Switch Debounce 2
;
; This shows one method to debounce switches.  
; Samples the line every 1mS, and waits for 5 in a row before
; acting on the change of state.

	
#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock	0x20
Delay		; Assign an address to label Delay1
Display 	; define a variable to hold the diplay
LastStableState	; keep track of switch state (open-1; closed-0)
Counter
	endc
	
	org 0
Start
	bsf	STATUS,RP0	; select Register Page 1
	movlw	0xFF
	movwf	TRISA		; Make PortA all input
	clrf	TRISC		; Make PortC all output

	bcf	STATUS,RP0	; address Register Page 2
	bsf	STATUS,RP1	
	movlw	0xF7		; PortA3 pin is digital
	movwf	ANSEL
	bcf	STATUS,RP0	; address Register Page 0
	bcf	STATUS,RP1
	
	clrf	Display
	movlw	1
	movwf	LastStableState	; Assume the Switch is up.
	clrf	Counter
MainLoop
	btfsc	LastStableState,0
	goto	LookingForUp
LookingForDown
	clrw			; assume it's not, so clear
	btfss	PORTA,3		; wait for switch to go low
	incf	Counter,w	; if it's low, bump the counter
	movwf	Counter		; store either the 0 or incremented value
	goto	EndDebounce
	
LookingForUp
	clrw			; assume it's not, so clear
	btfsc	PORTA,3		; wait for switch to go low
	incf	Counter,w
	movwf	Counter

EndDebounce
	movf	Counter,w	; have we seen 10 in a row?
	xorlw	5
	btfss	STATUS,Z	
	goto	Delay1mS
	
	comf	LastStableState,f	; after 10 straight, reverse the direction
	clrf	Counter
	btfss	LastStableState,0	; Was it a key-down press?
	goto	Delay1mS	; no: take no action
	
	incf	Display,f	; if it's the down direction, 
	movf	Display,w	; take action on the switch
	movwf	PORTC		; (increment counter and put on display)
	
Delay1mS
	movlw	.71		; delay ~1000uS
	movwf	Delay
	decfsz	Delay,f		; this loop does 215 cycles
	goto	$-1		
	decfsz	Delay,f		; This loop does 786 cycles
	goto	$-1
	goto	MainLoop
	end
