; +----------------------------+
; | Application pour rigoler ! |
; +----------------------------+
	
	; +------------------------+
	; | Quelle bête utilisée ? |
	; +------------------------+
	LIST      p=10F206
	#include <p10F206.inc>
	
	; +-----------+
	; | La config |
	; +-----------+
	__CONFIG   _MCLRE_OFF & _CP_OFF & _WDT_OFF


	; +--------+
	; | Define |
	; +--------+
#define LEDIR_ON	bcf	GPIO, 2
#define LEDIR_OFF	bsf GPIO, 2

;#define LED_ON		bcf	GPIO, 2
;#define LED_OFF		bsf GPIO, 2

#define	ALIM_ON		bsf GPIO, 1
#define	ALIM_OFF	bcf GPIO, 1

#define	FREQ_30KHZ	1
#define	FREQ_36KHZ	2
#define	FREQ_40KHZ	4


	; +-------+
	; | Macro |
	; +-------+
F_30KHZ macro
	clrf	freq
	bsf		freq, FREQ_30KHZ
endm

F_36KHZ macro
	clrf	freq
	bsf		freq, FREQ_36KHZ
endm

F_40KHZ macro
	clrf	freq
	bsf		freq, FREQ_40KHZ
endm


	; +-------------------+
	; | Zone de variables |
	; +-------------------+
  	CBLOCK  	0x08
		cmpt : 1
		bcl0 : 1
		bcl1 : 1

		time_1 : 1	; Temps à l'état haut (modulé)
		time_0 : 1	; Temps à l'état bas (absence de modulation)

		freq : 1	; Fréquence courante
		freq_ : 1	; Temporaire

		state : 1	; Si > 0, on module en permanence
 	ENDC


	org		0x00

	movwf	OSCCAL
	bcf		OSCCAL, FOSC4

	; GP0 et GP3 en entrée
	movlw	b'00001001'
	tris	GPIO

	movlw	0
	movwf	GPIO

	movlw	b'11000000'
	option

	bcf		CMCON0, CMPON



	;ALIM_ON

;go
;	LEDIR_ON
;	LEDIR_OFF
;	goto	go

;toto
;	call	send_grundig

;	call	tempo_10ms
;	call	tempo_10ms

;	call	tempo_10ms
;	call	tempo_10ms

;	call	tempo_10ms
;	call	tempo_10ms

;	goto	toto
main



;	call	send_rc5
;	call	tempo_10ms
;	call	tempo_10ms
;
;	call	send_sirc
;	call	tempo_10ms
;	call	tempo_10ms
;
;	goto	main

;	bcf		state, 0

;	bsf		GPIO, 1
;
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms
;
;	bcf		GPIO, 1
;
;toto
;	goto	toto	

; HOP
goto	send

	; Tant que GP0 est pressé, on module
;	btfsc	GPIO, 0
;	goto	send

;	call	tempo_10ms
;	call	tempo_10ms

	; Si GP3 est pressé, on module en permanence
;	btfsc	GPIO, 3
;	goto	send_m

;	btfss	GPIO, 0
;	goto	stop

	goto	main

send_m
	bsf		state, 0

send

	call	send_sirc
	call	tempo_10ms
	call	tempo_10ms

	call	send_sirc
	call	tempo_10ms
	call	tempo_10ms

	call	send_grundig_h
	call	tempo_10ms
	call	tempo_10ms

	call	send_grundig
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms

	call	send_rc5
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms

; HOP
goto send

	; Doit-on moduler encore ?
	btfsc	state, 0
	goto	test_state

	goto	main

test_state

	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms
	call	tempo_10ms

	btfsc	GPIO, 0
	goto	main

	btfss	GPIO, 3
	goto	send

stop
	ALIM_OFF
	goto	stop

;sender
;	LEDIR_OFF
;	LEDIR_ON
;	goto	sender


;	call	send_grundig_h
;	call	tempo_10ms
;	call	send_grundig
;	call	tempo_10ms
;	call	tempo_10ms
;	call	tempo_10ms

;	call	send_sony2

;	call	tempo_10ms
;	call	tempo_10ms

;	goto	main


#include "grundig.asm"
#include "rc5.asm"
#include "sirc.asm"
;#include "sony.asm"



; Envoie 3 trames de '1'
;send_3_1
;	movf	time_1, W
;	addwf	time_1, W
;	addwf	time_1, W
;	movwf	cmpt
;	goto	_send_1_next

; Envoie 2 trames de '1'
send_2_1
	movf	time_1, W
	addwf	time_1, W
	movwf	cmpt
	goto	_send_1_next

; Envoie une trame de '1'
send_1
	movf	time_1, W
	movwf	cmpt

_send_1_next

	; On teste la fréquence d'émission avant
	btfsc	freq, FREQ_30KHZ
	goto	_send_1_30khz

	btfsc	freq, FREQ_36KHZ
	goto	_send_1_36khz

	btfsc	freq, FREQ_40KHZ
	goto	_send_1_40khz

	return

; Envoie une trame à "0"
send_0
	movf	time_0, W
	movwf	cmpt

	; On teste la fréquence d'émission avant
	btfsc	freq, FREQ_30KHZ
	goto	_send_0_30khz

	btfsc	freq, FREQ_36KHZ
	goto	_send_0_36khz

	btfsc	freq, FREQ_40KHZ
	goto	_send_0_40khz

	return


; Tempo de 10ms !
tempo_10ms
	movlw	.58
	movwf	bcl1
_tempo_10ms_next
	movwf	bcl0
_tempo_10ms_bcl
	decfsz	bcl0
	goto	_tempo_10ms_bcl
	decfsz	bcl1
	goto	_tempo_10ms_next
	return


; On module à 30khz
_send_1_30khz

	LEDIR_ON

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
;	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop
;	nop

	decfsz	cmpt
	goto	_send_1_30khz
	return

_send_0_30khz

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
;	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop
;	nop

	decfsz	cmpt
	goto	_send_0_30khz
	return


; On module à 36khz
_send_1_36khz

	LEDIR_ON

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	decfsz	cmpt
	goto	_send_1_36khz
	return

_send_0_36khz

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop
	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	decfsz	cmpt
	goto	_send_0_36khz
	return


; On module à 40khz
_send_1_40khz

	LEDIR_ON

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop

	decfsz	cmpt
	goto	_send_1_40khz
	return

_send_0_40khz

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	nop

	LEDIR_OFF

	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop

	decfsz	cmpt
	goto	_send_0_40khz
	return



	END

