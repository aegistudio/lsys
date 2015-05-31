section .bss
video_line_counter		resd	1
video_column_counter		resd	1
video_line_length		equ	80
video_line_count		equ	50

section .text
global video_clear_screen
video_clear_screen:
	mov	dword[video_line_counter],	0
	mov	dword[video_column_counter],	0
	mov	edx,	0		; i = 0
	video_clear_screen.lineloop:
	cmp	edx,	video_line_length * video_line_count
	jge	video_clear_screen.lineend	; for(; i < video_line_count * video_line_length; i += video_line_length)
	mov	ecx,	0		; j = 0
		video_clear_screen.columnloop:
		cmp	ecx,	video_line_length
		jge	video_clear_screen.columnend	; for(int j = 0; j < video_line_length; i += 4){
		mov	ebx,	ecx
		add	ebx,	edx			; current_pos = i + j
		mov	dword[gs : ebx],	0	; *((dword*)current_pos) = 0
		add	ecx,	4
		jmp	video_clear_screen.columnloop
		video_clear_screen.columnend:		;}
	add	edx,	video_line_length
	jmp video_clear_screen.lineloop
	video_clear_screen.lineend:
	ret

global video_set_cursor
video_set_cursor:
	pusha
	mov	eax,	esp
	pop	dword[video_line_counter]
	pop	dword[video_column_counter]
	mov	esp,	eax
	popa
	ret
