video_line_length		equ	80
video_line_count		equ	25

section .text
global asm_video_brush_screen
;asm_video_brush_screen(dword and, dword or, dword index_first, dword index_second);
asm_video_brush_screen:
	push ebp
	mov ebp, esp
	pusha

	mov	ebx,	dword[ss : (ebp + 16)]		; i = index_first
	mov	edx,	dword[ss : (ebp + 20)]
	video_clear_screen.lineloop:
	cmp	ebx,	edx
	jge	video_clear_screen.lineend	; for(; i < index_second; i += 4){

	mov	eax,	dword[gs : ebx]		; dword brushed = *((dword*)current_pos);
	mov	esi,	dword[ss : (ebp + 8)]
	and	eax,	esi			; brushed &= and;
	mov	esi,	dword[ss : (ebp + 12)]
	or	eax,	esi			; brushed |= or;

	mov	dword[gs : ebx], eax		; *((dword*)current_pos) = brushed;
	add	ebx,	4
	jmp video_clear_screen.lineloop

	video_clear_screen.lineend:
	popa
	mov esp, ebp
	pop ebp
	ret

global asm_video_put_char
;asm_video_put_char(dword index, dword sequence)
asm_video_put_char:
	push	eax
	push	ecx
	mov	eax, [ss : (esp + 12)]	; The Index
	mov	ecx, [ss : (esp + 16)]	; The Sequence
	mov	byte[gs : (eax + 0)], cl
	mov	byte[gs : (eax + 1)], ch
	pop	eax
	pop	ecx
	ret
