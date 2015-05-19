org 07c00h
bootSectorCodeJump:
	; A Short Jump To The Boot Code. NOP Should Not Be Ignored!
	jmp short bootCodeBody
	nop

%include "boot/fat12header.inc"

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
		call floppy.readSector

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
		call floppy.readSector

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
		call floppy.readSector
		
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
		jmp bootCodeBody.readLoader
		bootCodeBody.readLoader.end:
	pop es
	mov ax, loaderLaunching
	mov bx, loaderLaunching.length
	call bootDisplayString

	jmp loader.base : loader.offset

%include "boot/floppy.inc"

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
