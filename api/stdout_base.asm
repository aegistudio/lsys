global asm_systemcall_stdout
extern video_put_char
extern video_put_string
extern video_clear_screen
extern video_set_cursor
extern systemcall_stdout_putinteger
section .text:
stdout_address_table:
	dd	stdout_putchar
	dd	stdout_putstring
	dd	stdout_clear
	dd	stdout_cursor
	dd 	stdout_putinteger

%include "include/interrupt.inc"
extern asm_interrupt_load_dataregs
asm_systemcall_stdout:
	cli
	push 0
	int_save
	call asm_interrupt_load_dataregs

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
	stdout_putinteger:
		push video_put_string
		push esi
		push edi
		call systemcall_stdout_putinteger
		add esp, 12
		jmp stdout_endofcall
	stdout_endofcall:

	int_restore
	add esp, 4
	sti
	iretd

