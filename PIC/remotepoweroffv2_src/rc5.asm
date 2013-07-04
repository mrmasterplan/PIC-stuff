
; Philips, Samsung, Schneider
; Normalement :
; Orion, Alba, Audiosonic
; Baur, Beon, Bush
; Cathay, Centurion, Clarivox, Crown
; Daewoo, Dansai, Digatron, Dixi
; Elin, Erres
; Fergusson, Finlux, Formenti
; GEC, Goldstar, Goodmans, Granada...
; RC5 coding (http://www.xs4all.nl/~sbp/knowledge/ir/rc5.htm)
; freq			:	[36khz]		-> 36 khz
; header		:	[0]			-> Pas d'entête
; data timing	:	[0]			-> Durées identiques pour 0 et 1
;					[889us]		-> Durée d'un bit
; data			:	[26]		-> Nombre de bits à envoyer
; GOOD				[1011 0101 0101 0101 0100 1011 01]
;					[0101 0110 1010 1010 1010 0101 1010]
send_rc5

	F_36KHZ

	movlw	0x21
	movwf	time_0
	movwf	time_1

	call	send_0
	call	send_1
	call	send_0
	call	send_1

	call	send_0
	call	send_2_1
	call	send_0

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	call	send_0
	call	send_1
	call	send_0
	call	send_1

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	return


;	call	send_1
;	call	send_0
;	call	send_2_1
;
;	call	send_0
;	call	send_1
;	call	send_0
;	call	send_1
;
;	call	send_0
;	call	send_1
;	call	send_0
;	call	send_1
;
;	call	send_0
;	call	send_1
;	call	send_0
;	call	send_1
;
;	call	send_0
;	call	send_1
;	call	send_0
;	call	send_0
;
;	call	send_1
;	call	send_0
;	call	send_2_1
;
;	call	send_0
;	call	send_1
;
