;*********************************************************
;
;

floppy.resetDriver:
	pusha
	xor ax, ax
	mov dl, byte[driverNumber]
	int 13h
	popa
	ret

floppy.readSector:
	; Parameters:
	; -	AL = sectors to read
	; -	CX = sectorId
	; -	ES:BX = buffer
	; Equations:
	; -	sectorPerTrack * trackId + (beginSector - 1) = sectorId.
	; -	cylinderId * heads + headId = trackId.

	pusha
	call floppy.resetDriver
	push ax

	mov ax, cx	; Prepare Division For sectorId.
	mov dx, word[sectorsPerTrack]
	div dl		; AL = trackId, AH = beginSector - 1
	inc ah		; AH = beginSector
	mov cl, ah	; CL = beginSector

	xor ah, ah	; AX = trackId
	mov dx, word[heads]
	div dl		; AL = cylinderId, AH = headId
	mov ch, al	; CH = cylinderId
	mov dh, ah	; DH = headId
	mov dl, byte[driverNumber]

	floppy.readSector.reading:
		pop ax		; Number Of Sectors To Read.
		push ax
		mov ah, 02h	; Using Read Sector Function.
		int 13h		; Interrupt Floppy I/O.
	jc floppy.readSector.reading	;While Reading Fault, Repeat.

	pop ax
	popa
	ret

