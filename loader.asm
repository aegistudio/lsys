org 0100h
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

	inLoaderNow db "You're now inside loader!!!"
	inLoaderNow.length equ $ - inLoaderNow
