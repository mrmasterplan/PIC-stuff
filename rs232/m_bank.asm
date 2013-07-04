;***************************************************************************
;                                                                     
;	Standard Macros for PIC 16XXX  V2.00
;	====================================
;
;	written by Peter Luethi, 08.01.1999, Dietikon, Switzerland
;	http://www.electronic-engineering.ch
;	last update: 20.08.2004
;
;***************************************************************************
#DEFINE	M_BANK_ID dummy

BANK0	macro			; select register bank 0
	bcf     STATUS,RP0
	bcf     STATUS,RP1
	endm
	
BANK1	macro			; select register bank 1
	bsf     STATUS,RP0
	bcf     STATUS,RP1
	endm

BANK2	macro			; select register bank 2
	bcf     STATUS,RP0
	bsf     STATUS,RP1
	endm

BANK3	macro			; select register bank 3
	bsf     STATUS,RP0
	bsf     STATUS,RP1
	endm


;*** Conditional branching ***
; Macros to simplify 'IF THEN ELSE' queries
; Pre:  valid w
; Post: compare w vs. m_val, branch to m_target or not

;branch on equal w and m_val
BEQ	macro	m_val, m_target
	sublw	m_val
	bz	m_target	; zero bit set, branch
	endm

;branch on 0 value in register m_file, jump to m_target
BZF	macro	m_file, m_target
	tstf	m_file		; check register (Z)
	bz	m_target	; zero bit set, branch
	endm	

;branch on not equal w and m_val
BNEQ	macro	m_val, m_target
	sublw	m_val
	bnz	m_target	; zero bit not set, branch
	endm

;branch on not 0 value in register m_file, jump to m_target
BNZF	macro	m_file, m_target
	tstf	m_file		; check register (Z)
	bnz	m_target	; zero bit set, branch
	endm	

;branch on greater w than m_val
BRG	macro	m_val, m_target
	sublw	m_val		; result = m_val - w
	bnc	m_target	; no carry if result negative, branch
	endm

;branch on equal or greater w than m_val
BREG	macro	m_val, m_target
	sublw	m_val-0x1	; result = (m_val-1) - w
	bnc	m_target	; no carry if result negative, branch
	endm

;branch on smaller w than m_val
BRS	macro	m_val, m_target
	sublw	m_val-0x1	; result = (m_val-1) - w
	bc	m_target	; carry if result zero or positive, branch
	endm

;branch on equal or smaller w than m_val
BRES	macro	m_val, m_target
	sublw	m_val		; result = m_val - w
	bc	m_target	; carry if result zero or positive, branch
	endm

;*** Microchip Tips'n Tricks ***
; swaps the contents of W and REG without using a second register
; from 'Microchip Tips'n Tricks'

SWAPWF macro REG
	XORWF	REG,F
	XORWF	REG,W
	XORWF	REG,F
	endm
