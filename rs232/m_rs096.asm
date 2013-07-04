;***************************************************************************
;                                                                     
;	RS232 Software Interface for PIC 16XXX
;	======================================
;
;	written by Peter Luethi, 31.01.1999, Dietikon, Switzerland
;	http://www.electronic-engineering.ch
;	last update: 14.05.2004
;
;	V1.02:	Changed error handling to dedicated ISR termination label 
;		_ISR_RS232error required at the end of the ISR
;		(11.04.2004)
;	V1.01:	Clean-up and improvements (30.12.2000)
;	V1.00:	Initial release (31.01.1999)
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
;	Processor:		Microchip PIC 16F84
;	Clock:			4.00 MHz XT
;	Throughput:		1 MIPS
;	Serial Rate:		9600 baud, 8 bit, no parity, 1 stopbit
;	Required Hardware:	RS232 level shifter (MAX232)
;	
;
;	DESCRIPTION:
;	============
;	Developed and tested on PIC 16F84, executeable on all interrupt
;	featured PICs.
;	Program handles all aspects of
;		Transmission (Register TXD) and
;		Reception (Register RXD) through interrupt.
;
;	Waitcycle for 9600 baud ==> 104 us each bit
;
;	Call of implemented procedures with:
;		"RS232init"	Initialization
;		"SEND	'c'"	sends character 'c'
;		"SENDw"		sends content of working register
;		"RECEIVE"	macro in ISR: receive from RS232, store in RXD
;
;
;	DECLARATIONS needed in MAIN PROGRAM:
;	====================================
;	ORG     0x00			; processor reset vector
;  	goto    MAIN			; main program
;
;	ORG     0x04			; interrupt vector
;	goto	ISR			; Interrupt Service Routine (ISR)
;
;	CONSTANT BASE = 0x0C		; base address of user file registers
;	TXD	equ	BASE+?		; TX-Data register
;	RXD	equ	BASE+?		; RX-Data register
;
;	#define	TXport	PORTA,0x00	; RS232 output port, could be
;	#define	TXtris	TRISA,0x00	; any active push/pull port
;	; RS232 input port is RB0, because of its own interrupt flag
;
;	Example code snippet of main program:
;	-------------------------------------
;	FLAGreg	equ	BASE+d'7'
;	#define	RSflag	FLAGreg,0x00	; RS232 data reception flag
;
;	RSservice			; RS232 service sub-routine
;		movfw	RXD		; get received RS232 data
;		<do whatever you want...>
;		bcf	RSflag		; reset RS232 data reception flag
;		bsf	INTCON,INTE	; re-enable RB0/INT interrupt
;		RETURN
;		
;	MAIN	RS232init		; RS232 Initialization
;		clrf	FLAGreg		; initialize all flags
;
;	LOOP	btfsc	RSflag		; check RS232 data reception flag
;		call	RSservice	; if set, call RS232 echo & LCD display routine
;		goto	LOOP
;
;		END
;
;	Example code snippet of ISR (to be implemented in main program):
;	----------------------------------------------------------------
;	;***** INTERRUPT SERVICE ROUTINE *****
;	ISR	<... context save ...>
;
;		;*** determine origin of interrupt ***
;		btfsc	INTCON,INTF	; check for RB0/INT interrupt
;		goto	_ISR_RS232	; if set, there was a keypad stroke
;	
;		<... check other sources, if any ...>
;
;		; catch-all
;		goto	ISRend		; unexpected IRQ, terminate execution of ISR
;	
;		;*** RS232 DATA ACQUISITION ***
;	_ISR_RS232
;		; first, disable interrupt source
;		bcf	INTCON,INTE	; disable RB0/INT interrupt
;		; second, acquire RS232 data
;		RECEIVE			; macro of RS232 software reception
;		bsf	RSflag		; enable RS232 data reception flag
;		goto	_ISR_RS232end	; terminate RS232 ISR properly
;
;		<... other ISR sources' handling section ...>
;
;		;*** ISR Termination ***
;		; NOTE: Below, I only clear the interrupt flags! This does not
;		; necessarily mean, that the interrupts are already re-enabled.
;		; Basically, interrupt re-enabling is carried out at the end of
;		; the corresponding service routine in normal operation mode.
;		; The flag responsible for the current ISR call has to be cleared
;		; to prevent recursive ISR calls. Other interrupt flags, activated
;		; during execution of this ISR, will immediately be served upon
;		; termination of the current ISR run.
;	_ISR_RS232error
;		bsf	INTCON,INTE	; after error, re-enable IRQ already here
;	_ISR_RS232end
;		bcf	INTCON,INTF	; clear RB0/INT interrupt flag
;		goto	ISRend		; terminate execution of ISR
;
;		<... other ISR sources' termination ...>
;
;	ISRend	<... context restore ...> ; general ISR context restore
;		RETFIE			; enable INTCON,GIE
;
;
;	REQUIRED MEMORY:
;	================
;	2 registers: @ BASE+0 - BASE+1
;
;***************************************************************************

;***** INCLUDE FILES *****

	IFNDEF	M_BANK_ID
		ERROR "Missing include file: m_bank.asm"
	ENDIF

;***** HARDWARE DECLARATION *****

	IFNDEF	TXport
		ERROR "Define TXport in MAIN PROGRAM."
	ENDIF
	IFNDEF	TXtris
		ERROR "Define TXtris in MAIN PROGRAM."
	ENDIF
	
	MESSG "Default RS232 RXport is PORTB,0x00."

	#define	RXport	PORTB,0x00	; Needs to be an interrupt supervised
	#define	RXtris	TRISB,0x00	; port! When modify, set adequate
					; flags in INTCON register.
	
;***** CONSTANT DECLARATION *****

	CONSTANT LF =	d'10'	; Line Feed
	CONSTANT CR =	d'13'	; Carriage Return
	CONSTANT TAB =	d'9'	; Tabulator
	CONSTANT BS =	d'8'	; Backspace
	
;***** REGISTER DECLARATION *****

	IFNDEF	BASE
	  ERROR "Declare BASE (Base address of user file registers) in MAIN PROGRAM"
	ENDIF

	TEMP1	set	BASE+d'0'	; universal temporary register
	TEMP2	set	BASE+d'1'

	IFNDEF	TXD
	  ERROR "Declare TXD register in MAIN PROGRAM"
	ENDIF
	IFNDEF	RXD
	  ERROR "Declare RXD register in MAIN PROGRAM"
	ENDIF	

;***** MACROS *****

RS232init macro
	BANK1
	bcf	TXtris		; set output
	bsf	RXtris		; set input with weak pull-up
	bcf	OPTION_REG,INTEDG ; RS232 interrupt on falling edge
	BANK0
	bsf	TXport		; set default state: logical 1
	bcf	INTCON,INTF	; ensure interrupt flag is cleared
	bsf	INTCON,INTE	; enable RB0/INT interrupt
	bsf	INTCON,GIE	; enable global interrupt
	endm

SEND	macro	S_string	; "SEND 'X'" sends character to RS232
	movlw	S_string
	call	SENDsub
	endm

SENDw	macro
	call	SENDsub
	endm

RECEIVE	macro
	call	SB_Wait		; first wait sub-routine
	btfsc	RXport
	goto	_RSerror	; no valid start bit
	movlw	0x08
	movwf	TEMP1		; number of bits to receive, 9600-8-N-1
_RECa	call	T_Wait		; inter-baud wait sub-routine
	btfsc	RXport
	bsf	RXD,0x07
	btfss	RXport
	bcf	RXD,0x07
	decfsz	TEMP1,w		; skip if TEMP1 == 1
	rrf	RXD,f		; do this only 7 times
	decfsz	TEMP1,f
	goto	_RECa
	call	T_Wait		; inter-baud wait sub-routine
	btfss	RXport		; check if stop bit is valid
	goto	_RSerror	; no valid stop bit
	endm

;***** SUBROUTINES *****

SENDsub	movwf	TXD		; store in data register
	bcf	TXport		; start bit
	movlw	0x08
	movwf	TEMP1		; number of bits to send, 9600-8-N-1
	call	T_Wait
_SENDa	btfsc	TXD,0x00	; send LSB first !
	bsf	TXport
	btfss	TXD,0x00
	bcf	TXport
	rrf	TXD,f
	call	T_Wait
	decfsz	TEMP1,f
	goto	_SENDa
	bsf	TXport		; stop bit
	call	T_Wait
	call	T_Wait		; due to re-synchronization
	RETURN

T_Wait	movlw	0x1D		; FOR TRANSMISSION & RECEPTION
	movwf	TEMP2		; total wait cycle until next
	goto	X_Wait		; bit: 9600 baud ==> 104 us

;*** When entering this subroutine, ISR context restore has already consumed some cycles ***
SB_Wait	movlw	0x08		; FOR RECEPTION of start bit
	movwf	TEMP2		; total wait cycle: 52 us
	goto	X_Wait		; (=> sampling in the center of each bit)
	
X_Wait	decfsz	TEMP2,f		; WAIT LOOP
	goto	X_Wait
	RETURN

_RSerror clrf	RXD		; invalid data
	goto	_ISR_RS232error	; goto RS232 error handling in ISR
