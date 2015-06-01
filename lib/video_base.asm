video_line_length		equ	80
video_line_count		equ	50

section .text
global asm_video_brush_screen
;asm_video_brush_screen(dword and, dword or, dword index_first, dword index_second);
asm_video_brush_screen:
	mov	ebx,	dword[ss : (esp + 12)]		; i = index_first
	mov	edx,	dword[ss : (esp + 16)]
	video_clear_screen.lineloop:
	cmp	ebx,	edx
	jge	video_clear_screen.lineend	; for(; i < index_second; i += 4){

	mov	eax,	dword[gs : ebx]		; dword brushed = *((dword*)current_pos);
	mov	esi,	dword[ss : (esp + 4)]
	and	eax,	esi			; brushed &= and;
	mov	esi,	dword[ss : (esp + 8)]
	or	eax,	esi			; brushed |= or;

	mov	dword[gs : ebx], eax		; *((dword*)current_pos) = brushed;
	add	ebx,	4
	jmp video_clear_screen.lineloop
	video_clear_screen.lineend:
	ret

global asm_video_put_char
;asm_video_put_char(dword index, dword sequence)
asm_video_put_char:
	mov	eax, [ss : (esp + 4)]	; The Index
	mov	ecx, [ss : (esp + 8)]	; The Sequence
	mov	byte[gs : (eax + 0)], cl
	mov	byte[gs : (eax + 1)], ch
	ret