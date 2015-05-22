org 0100h
jmp loaderCode
nop

%include "boot/fat12header.inc"

loaderCode:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	jmp $
