org 0100h
jmp loaderCode
nop

%include "boot/fat12header.inc"

loaderCode:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov ax, 1301h
	mov bx, 0007h
	mov cx, inLoaderNow.length
	mov dl, 0
	mov bp, inLoaderNow
	int 10h
	jmp $

	resb 3000h

	%include "boot/floppy.inc"

	inLoaderNow db "You're now inside loader!!!"
	inLoaderNow.length equ $ - inLoaderNow
