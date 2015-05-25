;********************************************************************
;			16 Bit Real Mode
; @Description: Now inside 16 bit real Mode Of Loader. Addressing uses
; 20 bit address line and segment registers. Now what we need to do is
; loading kernel into 0100h:0000h, setup descriptors and page tables
; then translate to protected mode.
;********************************************************************
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

	mov ax, selector.flat_data

	jmp dword selector.flat_code : loaderCode32

loader.displayString16:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 1301h
	mov bx, 000fh
	mov dx, word[loader.displayString.cursor]
	push dx
	int 10h
	pop dx
	mov dx, word[loader.displayString.lineLength]
	add word[loader.displayString.cursor], dx
	pop bp
	popa
	ret
	loader.displayString.cursor dw 0000h
	loader.displayString.lineLength dw 0100h

loader.writeToEnd:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov dx, word[loader.displayString.cursor]
	sub dx, 0100h
	mov bx, 0050h
	sub bx, cx
	or dx, bx
	mov bx, 0007
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
gdt.random_data descriptor	0,	0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.flat_code	descriptor	loader.physic_address, 0fffffh,		\
	descriptor.code32 | descriptor.code.readable |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.flat_data	descriptor	loader.physic_address,	0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.video	descriptor	0b8000h,	00ffffh,	\
	descriptor.data16 | descriptor.data.readwrite |\
	descriptor.gran.byte | descriptor.present,\
	privilege.user
gdt.stack	descriptor	loader.physic_address,	00ffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.putchar32.offset	equ	loader.putchar32 - $$
gdt.putchar32	gate selector.flat_code, gdt.putchar32.offset, 0,	\
	descriptor.system.386 | descriptor.gate.call | descriptor.present,\
	privilege.kernel
gdt.end:

gdt.register:
gdt.length	equ	gdt.end - gdt.base
loader.physic_address equ loader.base * 10h
	dw	gdt.length - 1
	dd	gdt.base + loader.physic_address

selector.random_data	equ	(gdt.random_data - gdt.base)|\
	selector.global | privilege.kernel
selector.flat_code	equ	(gdt.flat_code - gdt.base)|\
	selector.global | privilege.kernel
selector.flat_data	equ	(gdt.flat_data - gdt.base)|\
	selector.global | privilege.kernel
selector.video		equ	(gdt.video - gdt.base)|\
	selector.global | privilege.user
selector.stack		equ	(gdt.stack - gdt.base)|\
	selector.global | privilege.kernel
selector.putchar32	equ	(gdt.putchar32 - gdt.base)|\
	selector.global | privilege.kernel

;***********************************************************************
; *			32 Bit Protected Mode
; * @Description: Now inside 32 bit protected mode of loader, addressing
; * uses 32bit virtual address and descriptors.
;***********************************************************************
align 32
loaderCode32:
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov eax, selector.video
	mov gs, ax
	mov eax, selector.stack
	mov ss, ax
	mov ebp, loader.offset
	mov esp, ebp

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd32

	jmp $

abcdefg db "ABCDEFG"
length equ $ - abcdefg

loader.putchar32:
	; Parameter:
	; 	dx = The Character To Print
	;	ebx = The Position Of The Word On Buffer
	push eax
	shl ebx, 1
	mov eax, dword[gs : ebx]
	mov ah, dh
	mov al, dl
	mov dword[gs : ebx], eax
	inc ebx
	pop eax
	ret

loader.print32:
	loader.print32.string equ 4d
	loader.print32.limit equ 8d
	loader.print32.color equ 12d
	loader.print32.position equ 16d
	loader.print32.stacklimit equ 16d
	; Parameter:
	; 	ds : eax = The Address Of The String
	;	ebx = The Length Of The String
	;	cl = The Color Of The String
	;	edx = The Position Of The String
	pusha
	push ebp
	mov ebp, esp
	sub esp, loader.print32.stacklimit

	mov dword[ss : (ebp - loader.print32.string)], eax
	add ebx, eax
	mov dword[ss : (ebp - loader.print32.limit)], ebx
	mov byte[ss : (ebp - loader.print32.color)], cl
	mov dword[ss : (ebp - loader.print32.position)], edx

	loader.print32.print:
	mov eax, dword[ss : (ebp - loader.print32.string)]
	mov ebx, dword[ss : (ebp - loader.print32.limit)]
	cmp eax, ebx
	jge loader.print32.endprint
	mov dl, byte[ds : eax]
	mov dh, byte[ss : (ebp - loader.print32.color)]
	mov ebx, dword[ss : (ebp - loader.print32.position)]
	call loader.putchar32
	inc dword[ss : (ebp - loader.print32.string)]
	inc dword[ss : (ebp - loader.print32.position)]
	jmp loader.print32.print

	loader.print32.endprint
	add esp, loader.print32.stacklimit
	pop ebp
	popa
	ret

loader.displayString32:
	pusha
	mov edx, dword[ds : loader.displayString.cursor]
	call loader.print32
	add edx, 0050h
	mov dword[ds : loader.displayString.cursor], edx
	popa
	ret

loader.writeToEnd32:
	pusha
	mov cl, 07h
	mov edx, dword[ds : loader.displayString.cursor]
	shr edx, 2
	and edx, 0ffffh
	sub edx, ebx
	add edx, 0050h
	call loader.print32
	popa
	ret
