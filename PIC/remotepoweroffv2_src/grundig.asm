
; Grundig
; freq			:	[32khz]		-> 32 khz
; header		:	[1]			-> Entête présent
; 					[2,480ms]	-> Durée du 0
; 					[740us]		-> Durée du 1
; data timing	:	[1]			-> Durée différente pour 1 et 0
;					[580]		-> Durée du 0
;					[450]		-> Durée du 1
; data			:	[20]		-> Nombre de bits à envoyer
;					[1001 1001 0101 0101 0101]






; Grundig
; 32khz, espacé de l'entête de 20ms
; header : 740us à 1, 2,480ms à 0
; 1 bit 0 : 580us
; 1 bit 1 : 450us
; 1001 1001 0101 0101 0101
send_grundig

	F_30KHZ

	; Header
	movlw	.77
	movwf	time_0

	movlw	.18
	movwf	time_1

	call	send_1
	call	send_0

	movlw	.2
	movwf	time_1

	call	send_1

	; Data
	movlw	.17
	movwf	time_0

	movlw	.11
	movwf	time_1

	call	send_1

	movlw	.33
	movwf	time_0
	call	send_0

	movlw	.20
	movwf	time_0

	call	send_2_1

	call	send_0
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
	call	send_1

	return


send_grundig_h

	F_30KHZ

	; Header
	movlw	.77
	movwf	time_0

	movlw	.18
	movwf	time_1

	call	send_1
	call	send_0

	; Data
	movlw	.17
	movwf	time_0

	movlw	.11
	movwf	time_1

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

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	call	send_1
	call	send_0
	call	send_1
	call	send_0

	return
