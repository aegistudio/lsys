;*****************************************************************
; *			boot/floppy.inc
; * @Title: floppy function assembly include file
; * @Author: LuoHaoran
; * @Description: This file defines some shared function for floppy
; * device operations. Using __no_***__ marcos can intelligently
; * disable some function from writting to the code.
;*****************************************************************

%ifdef __fat12header__

%ifndef __no_reset_driver__
floppy.resetDriver:
	pusha
	xor ax, ax
	mov dl, byte[driverNumber]
	int 13h
	popa
	ret
%endif

%ifndef __no_read_sector__
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
%endif

%ifndef __no_read_file__
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
		mov dx, word[bp - floppy.readFile.currentCluster]

		and dl, 01h
		cmp dl, 0
		je floppy.readFile.fatIndexEven
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
%endif

%ifndef __no_find_directory__
floppy.findDirectory:
	floppy.findDirectory.returnValue equ 2
	floppy.findDirectory.areaSectorCounter equ 4
	floppy.findDirectory.areaBound equ 6
	floppy.findDirectory.offset equ 8
	floppy.findDirectory.fileName equ 10
	floppy.findDirectory.stackLimit equ 10
	; Parameters:
	; -	AX = the limit of the sectors.
	; -	CX = the first sector of the entry.
	; -	DS : DX = the file name to find.
	; -	ES : BX = the buffer.

	push di
	push si
	push dx
	push cx
	push bx
	push bp
	mov bp, sp
	sub sp, floppy.findDirectory.stackLimit

	mov word[bp - floppy.findDirectory.areaSectorCounter], cx
	mov word[bp - floppy.findDirectory.areaBound], ax
	mov word[bp - floppy.findDirectory.offset], bx
	mov word[bp - floppy.findDirectory.fileName], dx

	floppy.findDirectory.reading:
	; Load A Sector Into Memory
	mov cx, word[bp - floppy.findDirectory.areaSectorCounter]
	cmp cx, word[bp - floppy.findDirectory.areaBound]
	je floppy.findDirectory.notFound
	inc word[bp - floppy.findDirectory.areaSectorCounter]
	mov ax, 1
	mov bx, word[bp - floppy.findDirectory.offset]
	call floppy.readSector

	; Initializing Comparison
	mov si, word[bp - floppy.findDirectory.fileName]
	mov di, word[bp - floppy.findDirectory.offset]
	cld

	mov dx, rootEntry.count
	floppy.findDirectory.rootEntryComparison:
		cmp dx, 0
		jz floppy.findDirectory.reading
		dec dx

		mov cx, 11	; = fileName.length
		floppy.findDirectory.charComparison:
			cmp cx, 0
			jz floppy.findDirectory.found
			dec cx
			lodsb			; Load DS:SI Byte To AL
			cmp al, byte[es : di]
			jnz floppy.findDirectory.notTheSame
			inc di
			jmp floppy.findDirectory.charComparison
	floppy.findDirectory.notTheSame:
		and di, rootEntry.alignmentMask
		add di, rootEntry.size
		mov si, word[bp - floppy.findDirectory.fileName]
		jmp floppy.findDirectory.rootEntryComparison

	floppy.findDirectory.found:
		; Read The Loader File Into Memory.
		and di, rootEntry.alignmentMask
			; = ES + DI & Align = loader.base + DI & Align
		mov ax, word[es : di + 1ah]	; First Cluster Of Loader		
		jmp short floppy.findDirectory.return

	floppy.findDirectory.notFound:
		mov ax, 0
		jmp short floppy.findDirectory.return

	floppy.findDirectory.return:
		add sp, floppy.findDirectory.stackLimit
		pop bp
		pop bx
		pop cx
		pop dx
		pop si
		pop di
		ret
%endif

%ifndef __no_kill_motor__
floppy.killMotor:
	pusha
	mov dx, 03f2h
	mov al, byte[driverNumber]
	out dx, al
	popa
	ret
%endif

%else
	%error boot/fat12header.inc should be included before this file!
%endif
