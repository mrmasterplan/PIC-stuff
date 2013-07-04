

; Sony SIRC (Serial Infra Red Control)
; freq			:	[40khz]
; header		:	[2]			-> Entête à l'état haut
;					[2,4ms]
; data timing	:	[0]			-> Durées identiques pour 0 et 1
;					[600us]
; Le 1 étant représenté par des pulsations pendant 1,2ms suivi de rien pendant 600us
; Le 0 étant réprésenté par des pulsations pendant 600us suivi de rien pendant 600us
; data			:	[28]		-> Nombre de bits à envoyer
;					[0110 1011 0101 1010 1011 0101 0101]
send_sirc

	F_40KHZ

	; Envoie de l'entête
	movlw	.90
	movwf	time_1

	call	send_1

	; Préparation des timing
	movlw	.24
	movwf	time_0

	movlw	.24
	movwf	time_1

	; Envoie des bits
	call	send_0
	call	send_2_1
	call	send_0

	call	send_1
	call	send_0
	call	send_2_1

	call	send_0
	call	send_1
	call	send_0
	call	send_1

	call	send_1
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

	return
