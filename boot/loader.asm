;********************************************************************
;			16 Bit Real Mode
; @Description: Now inside 16 bit real Mode Of Loader. Addressing uses
; 20 bit address line and segment registers. Now what we need to do is
; loading kernel into 0050h:0000h, setup descriptors and page tables
; then translate to protected mode.
;********************************************************************
bits 16
org 0100h
jmp loaderCode
nop

%include "boot/fat12header.inc"

loaderCode:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov ax, nowInsideLoader
	mov bx, nowInsideLoader.length
	call loader.displayString16

	mov ax, findingKernel
	mov bx, findingKernel.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	mov dx, kernel.elf

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

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

nowInsideLoader db "You're now inside loader!!!"
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
