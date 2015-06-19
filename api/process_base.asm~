global asm_systemcall_process
extern scheduler_sleep
section .bss:
	asm_process_esp		resd	1
	asm_process_ss		resd	1
	asm_process_ldt		resd	1

section .text:
process_address_table:
	dd	process_yield
	dd	process_sleep
	dd	process_terminate

extern asm_interrupt_load_dataregs
asm_systemcall_process:
	cli
	push 0
	int_save
	call asm_interrupt_load_dataregs

	mov dword[asm_process_esp], esp
	mov word[asm_process_ss], ss
	sldt [asm_process_ldt]

	push asm_process_esp
	push asm_process_ss
	push asm_process_ldt

	push cs
	push dword[cs : process_address_table + (eax * 4)]
	retf

	process_yield:
		push 0
		call scheduler_sleep
		add esp, 4
		jmp process_endofcall
	process_sleep:
		push edi
		call scheduler_sleep
		add esp, 4
		jmp process_endofcall
	process_terminate:
		jmp process_endofcall
	process_endofcall:
	
	add esp, 12
	mov ebx, dword[asm_process_esp]
	mov ecx, dword[asm_process_ss]
	lldt [asm_process_ldt]

	mov ss, cx
	mov esp, ebx		; eax = return value (the return stack frame)
	add esp, 8
	pop es
	pop ds
	pop fs
	pop gs
	popad
	add esp, 4
	sti
	iretd

