;********************************************************************
;			16 Bit Real Mode
; @Description: Now inside 16 bit real Mode Of Loader. Addressing uses
; 20 bit address line and segment registers. Now what we need to do is
; loading kernel into 0100h:0000h, setup descriptors and page tables
; then translate to protected mode.
;********************************************************************
bits 16
org 0100h
jmp loaderCode
nop

%include "boot/fat12header.inc"
%include "protect.inc"

loaderCode:
	; Setup Registers
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	; Clear Screen
	mov ax, 0600h
	mov bx, 0700h
	mov cx, 0000h
	mov dx, 0184fh
	int 10h

	; Print Now Inside Loader
	mov ax, nowInsideLoader
	mov bx, nowInsideLoader.length
	call loader.displayString16

	;Print Finding Kernel
	mov ax, findingKernel
	mov bx, findingKernel.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Finding kernel.elf In Root Directory
	push es
	mov ax, 0100h
	mov es, ax

	mov dx, kernel.elf
	mov ax, rootEntry.areaSize + rootEntry.areaBeginSector
	mov cx, rootEntry.areaBeginSector
	mov bx, 0
	call floppy.findDirectory
	cmp ax, 0
	jne kernel.findSuccess

	; kernel.elf Not Found
	pop es
	mov ax, fail
	mov bx, fail.length
	call loader.writeToEnd
	jmp $

	; kernel.elf Found
	kernel.findSuccess:
	mov dx, es
	pop es
	push ax

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	; Display Loading Kernel
	mov ax, loadingKernel
	mov bx, loadingKernel.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Loading Kernel
	pop ax
	push es
	mov es, dx
	mov bx, 0

	call floppy.readFile

	mov dx,es
	pop es

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	push es

	jmp $

loader.displayString16:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov bx, 0007
	mov dx, word[loader.displayString.cursor]
	add word[loader.displayString.cursor], loader.displayString.lineLength
	int 10h
	pop bp
	popa
	ret
	loader.displayString.cursor dw 0
	loader.displayString.lineLength equ 80

loader.writeToEnd:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov bx, 0007
	mov dx, word[loader.displayString.cursor]
	sub dx, cx
	int 10h
	pop bp
	popa
	ret

%include "boot/floppy.inc"

kernel.elf db "KERNEL  ELF"

nowInsideLoader db "===================================LSYS 0.01===================================="
nowInsideLoader.length equ $ - nowInsideLoader

findingKernel db "Finding kernel..."
findingKernel.length equ $ - findingKernel

loadingKernel db "Loading kernel..."
loadingKernel.length equ $ - loadingKernel

progress db "      [    ]"
progress.length equ $ - progress
ok db "      [ OK ]"
ok.length equ $ - ok
fail db "      [FAIL]"
fail.length equ $ - fail

;***********************************************************************
; *			32 Bit Protected Mode
; * @Description: Now inside 32 bit protected mode of loader, addressing
; * uses 32bit virtual address and descriptors.
;***********************************************************************
bits 32
