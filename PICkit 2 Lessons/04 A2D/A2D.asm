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
; PICkit 2 Lesson 4 - "A2D"
;
; This shows how to read the A2D converter and display the
; High order parts on the 4 bit LED display.
; The pot on the Low Pin Count Demo board varies the voltage 
; coming in on in A0.
;
; The A2D is referenced to the same Vdd as the device, which 
; is provided by the USB cable and nominally is 5V.  The A2D
; returns the ratio of the voltage on Pin RA0 to 5V.  The A2D
; has a resolution of 10 bits, with 1023 representing 5V and
; 0 representing 0V.

#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock 0x20
Delay1		; Assign an address to label Delay1
Delay2
Display		; define a variable to hold the diplay
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
	movlw	0x01		; Make PortA bit 0 pin Analog
	movwf	ANSEL
	bcf	STATUS,RP0	; address Register Page 0
	bcf	STATUS,RP1
	
	movlw	0x01
	movwf	ADCON0		; configure A2D for Channel 0 (RA0), Left justified, and turn on the A2D module
MainLoop
	nop			; wait 5uS for A2D amp to settle and capacitor to charge.
	nop			; wait 1uS
	nop			; wait 1uS
	nop			; wait 1uS
	nop			; wait 1uS
	bsf	ADCON0,GO	; start conversion
	btfsc	ADCON0,GO	; this bit will change to zero when the conversion is complete
	goto	$-1

	swapf	ADRESH,w	; Copy the display to the LEDs
	movwf	PORTC
	goto	MainLoop
	end
	
