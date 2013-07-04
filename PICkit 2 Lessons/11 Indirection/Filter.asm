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

; PICkit 2 Lesson 11 - Moving Average Filter
;
; This shows using the FSR to implement a Moving Average filter
; it reads the pot value via the A2D converter, Averages the result
; with the last 7 readings and displays the High order bits on the 
; 4 bit LED display. The pot on the Low Pin Count Demo board varies 
; the voltage coming in on RA0.
;

#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock 0x20
Display		; define a variable to hold the diplay
Queue:8		; 8 bytes to hold last 8 entries
Delay:2		; counter to limit delay
RunningSum:2	; sum of last 8 entries
Round:2		; divide by 8 and round.
temp
	endc
		
	org 0
Start
	bsf	STATUS,RP0	; select Register Page 1
	movlw	0xFF
	movwf	TRISA		; Make PortA all input
	clrf	TRISC		; Make PortC all output
	movlw	0x10		; A2D Clock Fosc/8
	movwf	ADCON1
	bcf	STATUS,RP0	; back to Register Page 0

	bcf	STATUS,RP0	; address Register Page 2
	bsf	STATUS,RP1	
	movlw	0x01		; we want all Port A pins Analoga
	movwf	ANSEL
	bcf	STATUS,RP0	; address Register Page 0
	bcf	STATUS,RP1
	
	movlw	0x01
	movwf	ADCON0		; configure A2D for Channel 0 (RA0), Left justified, and turn on the A2D module
	call	FilterInit	; initialize the moving average filter
MainLoop
	call	Delay200mS
	bsf	ADCON0,GO	; start A2D conversion
	btfsc	ADCON0,GO	; this bit will change to zero when the conversion is complete
	goto	$-1

	movf	ADRESH,w	; read the A2D
	call	Filter		; send it to the filter
	movwf	Display		; save the filtered value
	swapf	Display,w		; swap the nybbles to put the high order
	movwf	PORTC		; into the low order nybble on Port C
	goto	MainLoop
	
; Third version - smaller and faster!
; rather than summing the array everytime, it keeps a running sum.
; Before inserting a new value into the queue, the oldest is subtracted
; from the running sum.  Then the new value is inserted into the array
; and added to the running sum.
; Assumes the FSR is not corrupted elsewhere in the program.  If the FSR
; may be used elsewhere, this module should maintain a copy for it's
; own use and reload the FSR before use.
FilterInit
	movlw	Queue
	movwf	FSR
	clrf	RunningSum
	clrf	RunningSum+1
	clrf	Queue
	clrf	Queue+1
	clrf	Queue+2
	clrf	Queue+3
	clrf	Queue+4
	clrf	Queue+5
	clrf	Queue+6
	clrf	Queue+7
	return
	
Filter
	movwf	temp	; save 
	
	movf	INDF,w		; subtract the current out of the sum
	subwf	RunningSum,f
	btfss	STATUS,C	; was there a borrow?
	decf	RunningSum+1,f	; yes, take it from the high order byte
	
	movf	temp,w
	movwf	INDF		; store in table
	addwf	RunningSum,f		; Add into the sum
	btfsc	STATUS,C
	incf	RunningSum+1,f
	
	incf	FSR,f
	movf	FSR,w
	xorlw	Queue+8		; did it overflow?
	movlw	Queue		; preload Queue base address (Does not affect the flags)
	btfsc	STATUS,Z
	movwf	FSR		; yes: reset the pointer

;MAStraightline  ; 53 instructions, 55 cycles including call and & return
	bcf	STATUS,C	; clear the carry
	rrf	RunningSum+1,w
	movwf	Round+1
	rrf	RunningSum,w	; divide by 2 and copy to a version we can corrupt
	movwf	Round
	
	bcf	STATUS,C	; clear the carry
	rrf	Round+1,f
	rrf	Round,f		; divide by 4

	bcf	STATUS,C	; clear the carry
	rrf	Round+1,f
	rrf	Round,f		; divide by 8
	
	btfsc	STATUS,C	; use the carry bit to round
	incf	Round,f		
	movf	Round,w		; load Wreg with the answer
	return	
	
; It's actually 197380uS including the Call and return.
Delay200mS
	decfsz	Delay,f	; delay 
	goto	$-1
	decfsz	Delay+1,f	; delay 768uS
	goto	$-3
	return
		
	end
