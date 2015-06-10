org 07c00h
bootSectorCodeJump:
	; A Short Jump To The Boot Code. NOP Should Not Be Ignored!
	jmp short bootCodeBody
	nop

%include "include/fat12header.inc"

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
		mov si, loader.bin
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
			mov si, loader.bin
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
		; Read The Loader File Into Memory.
		and di, rootEntry.alignmentMask
			; = ES + DI & Align = loader.base + DI & Align
		mov ax, word[es : di + 1ah]	; First Cluster Of Loader		
		mov bx, loader.offset
		call floppy.readFile
	pop es
	mov ax, loaderLaunching
	mov bx, loaderLaunching.length
	call bootDisplayString

	jmp loader.base : loader.offset

%define __no_find_directory__
%define __no_kill_motor__
%include "include/floppy.inc"

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

	rootEntry.areaSectorCounter dw rootEntry.areaBeginSector

	loader.bin db "LOADER  BIN"
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
