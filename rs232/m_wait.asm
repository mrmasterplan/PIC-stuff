;***************************************************************************
;                                                                     
;	Wait Routine for PIC 16XXX  V1.02
;	=================================
;
;	written by Peter Luethi, 18.01.1999, Dietikon, Switzerland
;	http://www.electronic-engineering.ch
;	last update: 20.08.2004
;
;	V1.02:	In case of 'WAIT 0x00' or 'WAITX 0x00, 0x0F', the wait
;		statement (macro) is ignored, no delay is executed
;		(05.06.2004)
;	V1.01:	Correction of a severe mistake: Programming of the prescaler
;		affected the entire OPTION_reg including PORTB pullups and
;		INTEDGE configuration. Now only the least significant 6 bits
;		of OPTION_reg are configured, the upper 2 bits are preserved.
;		(16.02.2003)
;	V1.00:	Initial release (18.01.1999)
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
;	Processor:		Microchip PIC 16X84, 16F7X, 16F87X
;	Clock:			4.00 MHz XT
;	Throughput:		1 MIPS
;
;
;	DESCRIPTION:
;	============
;	Developed on PIC 16C84, but executeable on all PIC 16X84, 16C7X, etc.
;	
;	Routine performs a "busy" wait.
;	Implemented standard delay (@ 4 MHz): 
;	"WAIT 0x01"	is equal to	1 Unit == 1.02 ms
;	The assigned standard prescaler for TMR0 is "PRESCstd = b'00000001'"
;	The prescaler is activated by clearing bit 3 of the OPTION register
;
;	Prescalers:
;		b'00000000' == 1:2	==>  0.512 ms
;		b'00000001' == 1:4	==>  1.02 ms	(standard)
;		b'00000111' == 1:256	==>  65.3 ms
;		Waittime = WCYCLE (= timeconst) * prescaler
;
;	Call of implemented procedures with:
;		"WAIT xxx"		; standard
;		"WAITX xxx, yyy"	; extended with specific prescaler
;					; WAITX	d'16',d'7' = 1.045 s @ 4 MHz
;
;
;	DECLARATIONS needed in MAIN PROGRAM:
;	====================================
;	"CONSTANT BASE = 0x0C"		Base address of user file registers
;
;
;	REQUIRED MEMORY:
;	================
;	1 register: @ BASE+0
;	needs itself 1 stack level
;
;***************************************************************************
#DEFINE	M_WAIT_ID dummy

;***** INCLUDE FILES *****

	IFNDEF	M_BANK_ID
	  ERROR "Missing include file: m_bank.asm"
	ENDIF

;***** CONSTANT DECLARATION *****
			
	CONSTANT PRESCstd = b'00000001'	; standard prescaler for TMR0
			
;***** REGISTER DECLARATION *****
			
	WCYCLE	set	BASE+d'0'	; wait cycle counter
	
;***** MACROS *****

WAIT	macro	timeconst_1
	IF (timeconst_1 != 0)
	  movlw	timeconst_1
	  call	WAITstd
	ENDIF
	endm

WAITX	macro	timeconst_2, prescaler
	IF (timeconst_2 != 0)
	  movlw	timeconst_2	
	  call	_Xsetup
	  movlw	prescaler	; assign prescaler for TMR0
	  call	_Xwait
	ENDIF
	endm
	
;***** SUBROUTINES *****

_Xsetup	movwf	WCYCLE		; assign wait cycle duration
	clrf	TMR0
	BANK1
	movlw	b'11000000'	; set up mask
	andwf	OPTION_REG,f	; clear corresponding bits
	RETURN

WAITstd	movwf	WCYCLE		; assign wait cycle duration
	clrf	TMR0
	BANK1
	movlw	b'11000000'	; set up mask
	andwf	OPTION_REG,f	; clear corresponding bits
	movlw	PRESCstd	; load standard prescaler
_Xwait	iorwf	OPTION_REG,f	; assign prescaler to TMR0
	BANK0

_WAITa	bcf	INTCON,T0IF	; clears TMR0 overflow flag
_WAITb	btfss	INTCON,T0IF	; checks TMR0 overflow flag, skip if set
	goto	_WAITb		; this is the wait-loop
	decfsz	WCYCLE,f	; repeats the loop according to the
	goto	_WAITa		; assigned wait cycles
	RETURN
