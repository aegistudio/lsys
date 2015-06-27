global asm_systemcall_semaphore
extern semaphore_init
extern semaphore_p
extern semaphore_v

section .bss:
	asm_semaphore_esp		resd	1
	asm_semaphore_ss		resd	1
	asm_semaphore_ldt		resd	1

section .text:
semaphore_address_table:
	dd	asm_semaphore_init
	dd	asm_semaphore_p
	dd	asm_semaphore_v

%include "include/interrupt.inc"
extern asm_interrupt_load_dataregs
asm_systemcall_semaphore:
	cli
	push 0
	int_save
	call asm_interrupt_load_dataregs
	
	mov dword[asm_semaphore_esp], esp
	mov word[asm_semaphore_ss], ss
	sldt [asm_semaphore_ldt]

	push asm_semaphore_esp
	push asm_semaphore_ss
	push asm_semaphore_ldt

	push cs
	push dword[cs : semaphore_address_table + (eax * 4)]
	retf

	asm_semaphore_init:
		push esi
		push edi
		call semaphore_init
		add esp, 8
		jmp semaphore_endofcall
	asm_semaphore_p:
		push edi
		call semaphore_p
		add esp, 4
		jmp semaphore_endofcall
	asm_semaphore_v:
		push edi
		call semaphore_v
		add esp, 4
		jmp semaphore_endofcall
	semaphore_endofcall:

	add esp, 12
	mov ebx, dword[asm_semaphore_esp]
	mov ecx, dword[asm_semaphore_ss]
	lldt [asm_semaphore_ldt]

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

