;********************************************************************
;			16 Bit Real Mode
; @Description: Now inside 16 bit real Mode Of Loader. Addressing uses
; 20 bit address line and segment registers. Now what we need to do is
; loading kernel into 0100h:0000h, setup descriptors and page tables
; then translate to protected mode.
;********************************************************************
bits 16
loader.base 	equ 9000h
loader.offset	equ 0100h
org 0100h
jmp loaderCode16
nop

%include "boot/fat12header.inc"
%include "protect.inc"

loaderCode16:
	; Setup Registers
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov bp, loader.offset
	mov sp, bp

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

	mov ax, killingMotor
	mov bx, killingMotor.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	call floppy.killMotor

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	mov ax, jumpingToProtectedMode
	mov bx, jumpingToProtectedMode.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Setting Up Protected Mode

	; Load Global Descriptor Table Pointer
	lgdt [gdt.register]

	; Open A20 Address Line
	cli
	in al, 92h
	or al, 00000010b
	out 92h, al

	; Set Control Register 0 : Protected = 1.
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp dword selector.flat_code : (loader.physic_address + loaderCode32)

loader.displayString16:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov bx, 0007
	mov dx, word[loader.displayString.cursor]
	push dx
	int 10h
	pop dx
	add dx, loader.displayString.lineLength
	and dx, 1fffh
	mov word[loader.displayString.cursor], dx
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

killingMotor db "Killing floppy motor..."
killingMotor.length equ $ - killingMotor

jumpingToProtectedMode db "Jumping to protected mode..."
jumpingToProtectedMode.length equ $ - jumpingToProtectedMode

progress db "      [    ]"
progress.length equ $ - progress
ok db "      [ OK ]"
ok.length equ $ - ok
fail db "      [FAIL]"
fail.length equ $ - fail

; Defining Descriptors
; Label		Type		Base	Limit	Attributes	Privilege
gdt.base	descriptor	0,	0,	0,		privilege.kernel
gdt.flat_code	descriptor	0,	0fffffh,	\
	descriptor.code32 | descriptor.code.readable |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.flat_data	descriptor	0,	0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.video	descriptor	0,	00ffffh,	\
	descriptor.data16 | descriptor.data.readwrite |\
	descriptor.gran.byte | descriptor.present,\
	privilege.user
gdt.end:

gdt.register:
gdt.length	equ	gdt.end - gdt.base
loader.physic_address equ loader.base * 10h + loader.offset
	dw	gdt.length - 1
	dd	gdt.base + loader.physic_address

selector.flat_code	equ	gdt.flat_code - gdt.base\
	| selector.global | privilege.kernel
selector.flat_data	equ	gdt.flat_data - gdt.base\
	| selector.global | privilege.kernel
selector.video		equ	gdt.video - gdt.base\
	| selector.global | privilege.kernel

;***********************************************************************
; *			32 Bit Protected Mode
; * @Description: Now inside 32 bit protected mode of loader, addressing
; * uses 32bit virtual address and descriptors.
;***********************************************************************
bits 32
loaderCode32:
	jmp $
