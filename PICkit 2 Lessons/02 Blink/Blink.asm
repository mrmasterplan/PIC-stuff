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
; PICkit 2 Lesson 2 - "Blink"
;
; First lesson showed how to make an LED turn on,
; Now we'll look at how to make it blink.  Delay loops are necessary
; to slow down the on and off commands so they are visible to humans.


#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock 0x20
Delay1			; Define two file registers for the
Delay2			; delay loop
	endc
	
	org 0
Start
	bsf	STATUS,RP0	; select Register Page 1
	bcf	TRISC,0		; make IO Pin B.0 an output
	bcf	STATUS,RP0	; back to Register Page 0
MainLoop
	bsf	PORTC,0		; turn on LED C0
OndelayLoop
	decfsz	Delay1,f	; Waste time.  
	goto	OndelayLoop	; The Inner loop takes 3 instructions per loop * 256 loopss = 768 instructions
	decfsz	Delay2,f	; The outer loop takes and additional 3 instructions per lap * 256 loops
	goto	OndelayLoop	; (768+3) * 256 = 197376 instructions / 1M instructions per second = 0.197 sec.
				; call it a two-tenths of a second.
	
	bcf	PORTC,0		; Turn off LED C0
OffDelayLoop
	decfsz	Delay1,f	; same delay as above
	goto	OffDelayLoop
	decfsz	Delay2,f
	goto	OffDelayLoop
	goto	MainLoop	; Do it again...
	end
	
