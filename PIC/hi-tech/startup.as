
	; HI-TECH C PRO for the PIC10/12/16 MCU family V9.60PL3
	; Copyright (C) 1984-2008 HI-TECH Software

	; Auto-generated runtime startup code for final link stage.

	;
	; Compiler options:
	;
	; --chip=16F690 code.C
	;


	processor	16F690

	global	_main,start,_exit,reset_vec
	fnroot	_main
	psect	config,class=CONFIG,delta=2
	psect	idloc,class=IDLOC,delta=2
	psect	rbss_0,class=BANK0,space=1
	psect	rbss_1,class=BANK1,space=1
	psect	rbss_2,class=BANK2,space=1
	psect	rbss_3,class=BANK3,space=1
	psect	rdata_0,class=BANK0,space=1
	psect	rdata_1,class=BANK1,space=1
	psect	rdata_2,class=BANK2,space=1
	psect	rdata_3,class=BANK3,space=1
	psect	nvram,class=BANK0,space=1
	psect	nvram_1,class=BANK1,space=1
	psect	nvram_2,class=BANK2,space=1
	psect	nvram_3,class=BANK3,space=1
	psect	nvbit_0,class=BANK0,bit,space=1
	psect	nvbit_1,class=BANK1,bit,space=1
	psect	nvbit_2,class=BANK2,bit,space=1
	psect	nvbit_3,class=BANK3,bit,space=1
	psect	temp,ovrld,class=BANK0,space=1
	psect	struct,ovrld,class=BANK0,space=1
	psect	code,class=CODE,delta=2
	psect	rbit_0,class=BANK0,bit,space=1
	psect	ptbit_0,class=BANK0,bit,space=1
	psect	rbit_1,class=BANK1,bit,space=1
	psect	rbit_2,class=BANK2,bit,space=1
	psect	rbit_3,class=BANK3,bit,space=1
	psect	strings,class=CODE,delta=2,reloc=256
	psect	pstrings,class=CODE,delta=2
	psect	powerup,class=CODE,delta=2
	psect	reset_vec,class=CODE,delta=2
	psect	maintext,class=CODE,delta=2
	C	set	0
	Z	set	2
	PCL	set	2
	INDF	set	0

	psect	fnautoc,class=COMMON,space=1
	psect	common,class=COMMON,space=1
	psect	fnauto0,class=BANK0,space=1
	psect	fnauto1,class=BANK1,space=1
	psect	fnauto2,class=BANK2,space=1
	STATUS	equ	3
	PCLATH	equ	0Ah

	psect	eeprom_data,class=EEDATA,delta=2,space=2
	psect	idata,class=CODE,delta=2
	psect	idata_0,class=CODE,delta=2
	psect	idata_1,class=CODE,delta=2
	psect	idata_2,class=CODE,delta=2
	psect	idata_3,class=CODE,delta=2
	psect	intcode,class=CODE,delta=2
	psect	intret,class=CODE,delta=2
	psect	intentry,class=CODE,delta=2
	global	intlevel0,intlevel1,intlevel2, intlevel3, intlevel4, intlevel5
intlevel0:
intlevel1:
intlevel2:
intlevel3:
intlevel4:
intlevel5:
	psect	intsave,class=BANK0,space=1
	psect	intsave_1,class=BANK1,space=1
	psect	intsave_2,class=BANK2,space=1
	psect	intsave_3,class=BANK3,space=1
	psect	init,class=CODE,delta=2
	psect	init23,class=CODE,delta=2
	psect	text,class=CODE,delta=2
	psect	end_init,class=CODE,delta=2
	psect	clrtext,class=CODE,delta=2
	psect	float_text0,class=CODE,delta=2,size=2048
	psect	float_text1,class=CODE,delta=2,size=2048
	psect	float_text2,class=CODE,delta=2,size=2048
	psect	float_text3,class=CODE,delta=2,size=2048
	psect	float_text4,class=CODE,delta=2,size=2048
	FSR	set	4

	psect	reset_vec
reset_vec:
	; No powerup routine
	; No interrupt routine


	psect	init
start
_exit
btemp	equ	0x70		;temporary memory only used by the following
FSR	set	4
INDF	set	0
PCLATH	set	0Ah
PC	set	2
global btemp, __Lramdata, __Lidata, __Hramdata
movlw	low(__Lramdata)
movwf	FSR
movlw	high(__Lidata)
movwf	btemp
movlw	low(__Lidata)
movwf	btemp+1
copyloop:
fcall fetch
movwf	INDF
incf	FSR,f
movf	FSR,w
xorlw	low(__Hramdata)
btfsc	STATUS,2
goto	done_copy
incf	btemp+1,f
btfsc	STATUS,2
incf	btemp,f
goto copyloop
fetch:
movf	btemp,w
movwf	PCLATH
movf	btemp+1,w
movwf	PC
done_copy:

;-------------------------------------------------------------------------------
;		Clear (zero) uninitialized global variables

;	No RAM objects to clear in bank 0
;	No RAM objects to clear in bank 1
;	No RAM objects to clear in bank 2
;	No RAM objects to clear in bank 3
;	2 bytes of RAM objects in common bank to zero

	psect	init
	bsf	STATUS,7		;select indirect bank 2/3
	global	__Lcommon
	movlw	low(__Lcommon)		;load the low address into FSR
	movwf	FSR
	movlw	low(__Lcommon+2)		;load the high address+1 into W
	fcall	clear_ram		;go and zero that range of addresses

	psect	clrtext,class=CODE,delta=2
	global	clear_ram
;	Called with FSR containing the base address, and
;	W with the last address+1
clear_ram:
	clrwdt			;clear the watchdog before getting into this loop
clrloop:
	clrf	INDF		;clear RAM location pointed to by FSR
	incf	FSR,f		;increment pointer to next location
	xorwf	FSR,w		;XOR with final address
	btfsc	STATUS,2	;have we reached the end yet?
	retlw	0		;all done for this bank, return
	xorwf	FSR,w		;XOR again to restore value
	goto	clrloop		;go and do the next byte


;-------------------------------------------------------------------------------
	psect	end_init
	ljmp _main
	end	start
