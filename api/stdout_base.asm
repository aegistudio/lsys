global asm_systemcall_stdout
extern video_put_char
extern video_put_string
extern video_clear_screen
extern video_set_cursor
section .text:
stdout_address_table:
	dd	stdout_putchar
	dd	stdout_putstring
	dd	stdout_clear
	dd	stdout_cursor

asm_systemcall_stdout:
	cli
	push eax
	push cs
	push dword[cs : stdout_address_table + (eax * 4)]
	retf

	stdout_putchar:
		push esi
		push edi
		call video_put_char
		add esp, 8
		jmp stdout_endofcall
	stdout_putstring:
		push esi
		push edi
		call video_put_string
		add esp, 8
		jmp stdout_endofcall
	stdout_clear:
		call video_clear_screen
		jmp stdout_endofcall
	stdout_cursor:
		push esi
		push edi
		call video_set_cursor
		add esp, 8
		jmp stdout_endofcall
	stdout_endofcall:
	pop eax
	sti
	iretd
