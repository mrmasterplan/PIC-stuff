;***************************************************************************
;
;	RS232 Test Interface V1.02
;	==========================
;
;	written by Peter Luethi, 26.03.1999, Dietikon, Switzerland
;	http://www.electronic-engineering.ch
;	last update: 16.01.2005
;
;	V1.02:	Fixed copy/paste issue of ISR context store/restore
;		(nobody is perfect): Erroneously erased INTCON,INTF
;		clearing, resulting in endless ISR calling...
;		Re-structured entire ISR and RS232 echo sub-routines
;		(11.04.2004)
;
;	V1.01:	ISR context restore improvements (21.11.1999)
;
;	V1.00:	Initial release (26.3.1999)
;
;	This code and accompanying files may be distributed freely and
;	modified, provided this header with my name and this notice remain
;	intact. Ownership rights remain with me.
;	You may not sell this software without my approval.
;
;	This software comes with no guarantee or warranty except for my
;	good intentions. By using this code you agree to indemnify me from
;	any liability that might arise from its use.
;
;
;	SPECIFICATIONS:
;	===============
;	Processor:			Microchip PIC 16F84A
;	Clock Frequency:		4.00 MHz XT
;	Throughput:			1 MIPS
;	Baud Rate:			depends on the module included
;	Code Size of entire Program:	approx. 235 instruction words
;	Required Hardware:		MAX 232
;
;
;	DESCRIPTION:
;	============
;	Developed and tested on PIC 16F84A, executeable on all interrupt
;	featured PICs.
;	Program handles all aspects of RS232
;		Transmission (Register TXD) and
;		Reception (register RXD) through interrupts (PortB0 IRQ).
;	The microcontroller sends feedback of received characters back to
;	the terminal window.
;
;	Program shows the implementation and function of the modules
;	m_bank.asm, m_wait.asm, and m_rs096.asm on the PIC16F84A.
;
;***************************************************************************

;***** COMPILATION MESSAGES & WARNINGS *****

	ERRORLEVEL -207 	; found label after column 1
	ERRORLEVEL -302 	; register in operand not in bank 0

;***** PROCESSOR DECLARATION & CONFIGURATION *****

	PROCESSOR 16F84A
	#include "p16f84a.inc"
	
	; embed Configuration Data within .asm File.
	__CONFIG   _CP_OFF & _WDT_OFF & _PWRTE_ON & _XT_OSC

;***** MEMORY STRUCTURE *****

	ORG     0x00			; processor reset vector
  	goto    MAIN

	ORG     0x04			; interrupt vector
	goto	ISR			; Interrupt Service Routine (ISR)

;***** PORT DECLARATION *****

	#define	TXport	PORTA,0x00	; RS232 output port, could be
	#define	TXtris	TRISA,0x00	; any active push/pull port

;***** CONSTANT DECLARATION *****

	CONSTANT BASE = 0x0C		; base address of user file registers	

;***** REGISTER DECLARATION *****

	FLAGreg	equ	BASE+d'7'
	#define	RSflag	FLAGreg,0x00	; RS232 data reception flag

	TXD	equ	BASE+d'8'	; TX-Data register
	RXD	equ	BASE+d'9'	; RX-Data register

	W_TEMP	equ	BASE+d'10'	; context register (ISR)
	STATUS_TEMP equ	BASE+d'11'	; context register (ISR)
	PCLATH_TEMP equ	BASE+d'12'	; context register (ISR)
	FSR_TEMP equ	BASE+d'13'	; context register (ISR)

;***** INCLUDE FILES *****

	#include "..\..\m_bank.asm"
	#include "..\..\m_wait.asm"
	#include "..\..\m_rs096.asm"	; specify desired RS232 module

;***** MACROS *****

;***** SUB-ROUTINES *****

RSservice 
	SEND	TAB
	SEND	'E'
	SEND	'c'
	SEND	'h'
	SEND	'o'
	SEND	':'
	SEND	TAB
	movfw	RXD		; get RS232 data
	SENDw			; transmit across RS232
	SEND	CR		; Carriage Return
	SEND	LF		; Line Feed
	; end of RS232 service (echo & display)
	bcf	RSflag		; reset RS232 data reception flag
	bsf	INTCON,INTE	; re-enable RB0/INT interrupt
	RETURN

;***** INTERRUPT SERVICE ROUTINE *****

ISR	;************************
	;*** ISR CONTEXT SAVE ***
	;************************

	bcf	INTCON,GIE	; disable all interrupts
	btfsc	INTCON,GIE	; assure interrupts are disabled
	goto	ISR
	movwf	W_TEMP		; context save: W
	swapf	STATUS,W	; context save: STATUS
	movwf	STATUS_TEMP	; context save
	clrf	STATUS		; bank 0, regardless of current bank
	movfw	PCLATH		; context save: PCLATH
	movwf	PCLATH_TEMP	; context save
	clrf	PCLATH		; page zero, regardless of current page
	bcf	STATUS,IRP	; return to bank 0
	movfw	FSR		; context save: FSR
	movwf	FSR_TEMP	; context save
	;*** context save done ***

	;**************************
	;*** ISR MAIN EXECUTION ***
	;**************************
	
	;*** check origin of interrupt ***
	btfsc	INTCON,INTF	; check for RB0/INT interrupt
	goto	_ISR_RS232	; if set, there was a keypad stroke

	; catch-all
	goto	ISRend		; unexpected IRQ, terminate execution of ISR

	;******************************
	;*** RS232 DATA ACQUISITION ***
	;******************************
_ISR_RS232
	; first, disable interrupt source
	bcf	INTCON,INTE	; disable RB0/INT interrupt
	; second, acquire RS232 data
	RECEIVE			; macro of RS232 software reception
	bsf	RSflag		; enable RS232 data reception flag
	goto	_ISR_RS232end	; terminate RS232 ISR properly

	;***********************************
	;*** CLEARING OF INTERRUPT FLAGS ***
	;***********************************
	; NOTE: Below, I only clear the interrupt flags! This does not
	; necessarily mean, that the interrupts are already re-enabled.
	; Basically, interrupt re-enabling is carried out at the end of
	; the corresponding service routine in normal operation mode.
	; The flag responsible for the current ISR call has to be cleared
	; to prevent recursive ISR calls. Other interrupt flags, activated
	; during execution of this ISR, will immediately be served upon
	; termination of the current ISR run.
_ISR_RS232error
	bsf	INTCON,INTE	; after error, re-enable IRQ already here
_ISR_RS232end
	bcf	INTCON,INTF	; clear RB0/INT interrupt flag
	;goto	ISRend		; terminate execution of ISR

	;*****************************************
	;*** ISR TERMINATION (CONTEXT RESTORE) ***
	;*****************************************

ISRend	movfw	FSR_TEMP	; context restore
	movwf	FSR		; context restore
	movfw	PCLATH_TEMP	; context restore
	movwf	PCLATH		; context restore
	swapf	STATUS_TEMP,W	; context restore
	movwf	STATUS		; context restore
	swapf	W_TEMP,F	; context restore
	swapf	W_TEMP,W	; context restore
	RETFIE			; enable global interrupt (INTCON,GIE)

;***** END OF INTERRUPT SERVICE ROUTINE *****


;************** MAIN **************

MAIN	
	clrf	INTCON		; reset interrupts (disable all)	
	RS232init		; RS232 initialization
	clrf	FLAGreg		; initialize all flags

	SEND	CR		; Carriage Return
	SEND	LF		; Line Feed
	SEND	'R'
	SEND	'S'
	SEND	'2'
	SEND	'3'
	SEND	'2'
	SEND	' '
	SEND	'T'
	SEND	'e'
	SEND	's'
	SEND	't'
	SEND	' '
	SEND	'I'
	SEND	'n'
	SEND	't'
	SEND	'e'
	SEND	'r'
	SEND	'f'
	SEND	'a'
	SEND	'c'
	SEND	'e'
	SEND	':'
	SEND	' '
	SEND	'P'
	SEND	'I'
	SEND	'C'
	SEND	'1'
	SEND	'6'
	SEND	'F'
	SEND	'8'
	SEND	'4'
	SEND	' '
	SEND	'c'
	SEND	'o'
	SEND	'n'
	SEND	'n'
	SEND	'e'
	SEND	'c'
	SEND	't'
	SEND	'e'
	SEND	'd'
	SEND	'.'
	SEND	'.'
	SEND	'.'
	SEND	CR		; Carriage Return
	SEND	LF		; Line Feed
	SEND	LF		; Line Feed

LOOP	btfsc	RSflag		; check RS232 data reception flag
	call	RSservice	; if set, call RS232 echo & LCD display routine
	goto	LOOP

	END
