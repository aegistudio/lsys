;*********************************************************
;
;

%ifdef __fat12header__

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

floppy.readFile:
	floppy.readFile.currentCluster equ 2	; word[bp - currentCluster]
	floppy.readFile.memoryPointer equ 4	; word[bp - memoryPointer]
	floppy.readFile.fatIndexInTable equ 6	; word[bp - fatIndexInTable]
	floppy.readFile.reservedBlockSize equ 6	; stack.length

	floppy.readFile.fatBufferBase equ 8000h
	floppy.readFile.fatBufferOffset equ 0000h

	; Parameters:
	; -	AX = first cluster of the file
	; -	ES : BX = buffer
	pusha
	push bp
	mov bp, sp
	sub sp, floppy.readFile.reservedBlockSize

	mov word[bp - floppy.readFile.memoryPointer], bx

	floppy.readFile.loop:
		;If It's Already The Last Cluster, End Of Reading.
		mov word[bp - floppy.readFile.currentCluster], ax
		cmp ax, 0FF0h
		jge floppy.readFile.loop.end

		sub ax, 2		; The Bias Of Cluster In Data Area
		mov dl, byte[sectorsPerCluster]
		mul dl
		add ax, data.areaBeginSector
		; SectorId = data.areaBeginSector 
		;	+ sectorsPerCluster * (clusterId - 2)
		mov cx, ax
		mov al, byte[sectorsPerCluster]
		mov bx, word[bp - floppy.readFile.memoryPointer]
			; ES:BX = ES : floppy.readFile.memoryPointer
		call floppy.readSector

		add word[bp - floppy.readFile.memoryPointer], data.bytesPerCluster

		; Now Read Into The FatTable.
		; Equations:
		; - byteIndexInFat = (word[currentCluster] >> 1) * 3
		; - fatTableToLoad = reservedSectors 
		;	+ (byteIndexInFat / (bytesPerSector))
		; - fatIndexInTable = byteIndexInFat % (bytesPerSector)
		mov ax, word[bp - floppy.readFile.currentCluster]
		shr ax, 1
		mov dl, 3
		mul dl
		mov bx, word[bytesPerSector]
		mov dx, 0
		div bx

		mov word[bp - floppy.readFile.fatIndexInTable], dx	; DX = fatIndexInTable
		add ax, word[reservedSectors]	; AX = fatTableToLoad

		push es
		mov cl, al
		mov bx, floppy.readFile.fatBufferOffset
		mov ax, floppy.readFile.fatBufferBase
		mov es, ax
		mov ax, 2
		call floppy.readSector
		
		mov si, word[bp - floppy.readFile.fatIndexInTable]
		mov dx, word[bp - floppy.readFile.fatIndexInTable]
		and dx, 01h
		jz floppy.readFile.fatIndexEven
		floppy.readFile.fatIndexOdd:
			mov ax, word[es : si + 1]
			shr ax, 4d		;Right Shift 4 bit When Is Odd
			jmp floppy.readFile.fatIndexEnd
		floppy.readFile.fatIndexEven:
			mov ax, word[es : si]
		floppy.readFile.fatIndexEnd:
		and ax, 0FFFh		;Masking 12 Bit.
		pop es
		jmp floppy.readFile.loop
	floppy.readFile.loop.end:

	add sp, floppy.readFile.reservedBlockSize
	pop bp
	popa
	ret

%else
	%error boot/fat12header.inc should be included before this file!
%endif
