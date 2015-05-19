org 07c00h
bootSectorCodeJump:
	; A Short Jump To The Boot Code. NOP Should Not Be Ignored!
	jmp short bootCodeBody
	nop

bootSectorAdditions:
	; Additonal Information In Boot Sector.
	oemName					db 'LHR'
	times 8-($-oemName)	 		db ' '
	biosParameterBlock:
		bytesPerSector			dw 0200h	
		sectorsPerCluster		db 01h
		reservedSectors			dw 0001h
		numberFats			db 02h
		rootEntries			dw 00e0h
		wordTotalSectors		dw 0b40h
		media				db 0f0h
		wordFatSize			dw 0009h
		sectorsPerTrack			dw 0012h
		heads				dw 0002h
		hiddenSectors			dd 00000000h
		dwordTotalSectors		dd 00000000h
	BiosParameterBlock.end:
	driverNumber				db 00h
	byteReserved				db 00h
	bootSign				db 29h
	volumeId				dd 00000000h
	volumeLabel				db 'LSYS0.01'
	times 11-($-volumeLabel)		db ' '
	fileSystemType				db 'FAT12'
	times 8-($-fileSystemType)		db ' '

	; When Any Of Above Entry Changes, This Section Will Be Recalculated.
	rootEntry.areaBeginSector equ 19
		; = numberFats * wordFatSize + reservedSectors.
	rootEntry.size equ 20h
	rootEntry.count equ 10h
		; = bytesPerSector / rootEntry.size
	rootEntry.areaSize equ 14
		; = ceil(rootEntries * rootEntry.size / bytesPerSector).
	data.areaBeginSector equ 33
		; = rootEntry.areaBeginSector + rootEntry.areaSize
	data.bytesPerCluster equ 0200h
		; = bytesPerSector * sectorsPerCluster

bootCodeBody:
	; Initialize Segment Registers.
	mov ax, cs
	;mov ds, ax
	mov es, ax
	mov ss, ax

	; Clear Screen
	mov ax, 0600h
	mov bx, 0700h
	mov cx, 0000h
	mov dx, 0184fh
	int 10h

	; Print Begin Loading Loader Message
	mov ax, beginLoadingSystemLoader
	mov bx, beginLoadingSystemLoader.length
	call bootDisplayString

	; As BootSectorAddtions Have Been Loaded, Just Need To Find
	; RootEntries Directly.
	; Read To 512 bytes after.
	push es
	mov ax, loader.base
	mov es, ax

	bootCodeBody.readRootEntryArea:
		; Load A Sector Into Memory
		mov cx, word[rootEntry.areaSectorCounter]
		cmp cx, rootEntry.areaBeginSector + rootEntry.areaSize
		je bootCodeBody.readRootEntryArea.notfound
		inc word[rootEntry.areaSectorCounter]
		mov ax, 1
		mov bx, loader.offset
		call bootReadSector

		; Initializing Comparison
		mov si, loaderFileName
		mov di, loader.offset
		cld

		mov dx, rootEntry.count
		bootCodeBody.rootEntryComparison:
			cmp dx, 0
			jz bootCodeBody.rootEntryComparison.end
			dec dx

			mov cx, 11	; = loaderFileName.length
			bootCodeBody.charComparison:
				cmp cx, 0
				jz bootCodeBody.readRootEntryArea.found
				dec cx
				lodsb			; Load DS:SI Byte To AL
				cmp al, byte[es : di]
				jnz bootCodeBody.rootEntryComparison.notTheSame
				inc di
				jmp bootCodeBody.charComparison
		bootCodeBody.rootEntryComparison.notTheSame:
			and di, rootEntry.alignmentMask
			add di, rootEntry.size
			mov si, loaderFileName
			jmp bootCodeBody.rootEntryComparison

		bootCodeBody.rootEntryComparison.end:
			jmp bootCodeBody.readRootEntryArea
	bootCodeBody.readRootEntryArea.notfound:
	pop es

	mov ax, loaderNotFound
	mov bx, loaderNotFound.length
	call bootDisplayString

	jmp $

	bootCodeBody.readRootEntryArea.found:
		and di, rootEntry.alignmentMask
			; = ES + DI & Align = loader.base + DI & Align
		mov ax, word[es : di + 1ah]	; First Cluster Of Loader

		bootCodeBody.readLoader:
		;If It's Already The Last Cluster, End Of Reading.
		mov word[loader.currentCluster], ax
		cmp ax, 0FF0h
		jg bootCodeBody.readLoader.end

		sub ax, 2		; The Bias Of Cluster In Data Area
		mov dl, byte[sectorsPerCluster]
		mul dl
		add ax, data.areaBeginSector
		; SectorId = data.areaBeginSector 
		;	+ sectorsPerCluster * (clusterId - 2)
		mov cx, ax
		mov al, byte[sectorsPerCluster]
		mov bx, word[loader.memoryPointer]
			; ES:BX = loader.base + loader.memoryPointer
		call bootReadSector

		add word[loader.memoryPointer], data.bytesPerCluster

		; Now Read Into The FatTable.
		; Equations:
		; - byteIndexInFat = (word[loader.currentCluster] >> 1) * 3
		; 	= currentCluster >> 1 + currentCluster
		; fatTableToLoad = reservedSectors 
		;	+ (byteIndexInFat / (bytesPerSector<<1))
		; fatIndexInTable = byteIndexInFat % (bytesPerSector<<1)
		push es
		mov ax, word[loader.currentCluster]
		shr ax, 1
		mov dl, 3
		mul dl
		mov bx, word[bytesPerSector]
		shl bx, 1			; DX = bytesPerSector<<1
		div bx

		; This is a regulation for division. (Error Not Known)
		sub ax, 00c0h

		mov word[loader.fatIndexInTable], dx	; DX = fatIndexInTable
		add ax, word[reservedSectors]	; AX = fatTableToLoad

		mov cl, al
		mov bx, 0000h
		mov ax, loader.fatBuffer	; Buffer = ES : BX = 8000 : 0000
		mov es, ax
		mov ax, 2
		call bootReadSector
		
		mov si, word[loader.fatIndexInTable]
		mov dx, word[loader.fatIndexInTable]
		and dx, 01h
		jz bootCode.fatIndexEven
		bootCode.fatIndexOdd:
			mov ax, word[es : si + 1]
			shr ax, 4d		;Right Shift 4 bit When Is Odd
			jmp bootCode.fatIndexEnd
		bootCode.fatIndexEven:
			mov ax, word[es : si]
		bootCode.fatIndexEnd:
		and ax, 0FFFh		;Masking 12 Bit.

		pop es
		;jmp bootCodeBody.readLoader
		bootCodeBody.readLoader.end:
	pop es
	mov ax, loaderLaunching
	mov bx, loaderLaunching.length
	call bootDisplayString

	jmp loader.base : loader.offset

bootResetDriver:
	pusha
	xor ax, ax
	mov dl, byte[driverNumber]
	int 13h
	popa
	ret

bootReadSector:
	; Parameters:
	; -	AL = sectors to read
	; -	CX = sectorId
	; -	ES:BX = buffer
	; Equations:
	; -	sectorPerTrack * trackId + (beginSector - 1) = sectorId.
	; -	cylinderId * heads + headId = trackId.

	pusha
	call bootResetDriver
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

	bootReadSector.reading:
		pop ax		; Number Of Sectors To Read.
		push ax
		mov ah, 02h	; Using Read Sector Function.
		int 13h		; Interrupt Floppy I/O.
	jc bootReadSector.reading	;While Reading Fault, Repeat.

	pop ax
	popa
	ret

bootDisplayString:
	; Parameters:
	; -	AX = address
	; -	BX = length
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov bx, 0007h
	mov dx, word[bootDisplayString.cursor]
	add word[bootDisplayString.cursor], bootDisplayString.lineLength
	int 10h
	pop bp
	ret
	bootDisplayString.cursor dw 0
	bootDisplayString.lineLength equ 80d

bootCodeConstants:
	loader.base equ 09000h
	loader.offset equ 0100h
	loader.memoryPointer dw 0100h
		; Pointer To The Bottom Of The Loaded Code.
	loader.currentCluster dw 0000h
		; The Current Cluster Of The Loader.
	loader.fatIndexInTable dw 0000h
		; The Next Cluster Of The Loader.
	loader.fatBuffer equ 08000h

	rootEntry.areaSectorCounter dw rootEntry.areaBeginSector
	rootEntry.alignmentMask equ 0FFE0h

	loaderFileName db "LOADER  BIN"
	beginLoadingSystemLoader db "Loading system loader..."
	beginLoadingSystemLoader.length equ $ - beginLoadingSystemLoader
	loaderNotFound db "Error: Loader Not Found!"
	loaderNotFound.length equ $ - loaderNotFound
	loaderLaunching db "Launching loader..."
	loaderLaunching.length equ $ - loaderLaunching

bootSectorFoot:
	; Padding Up The Boot Sector With Zeros And End Sign.
	padding times 510-($-$$)		db 0
	bootSectorSign				dw 0aa55h
