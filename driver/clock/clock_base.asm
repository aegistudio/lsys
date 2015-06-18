%include "include/interrupt.inc"

section .bss:
	asm_clock_save_esp		resd	1
	asm_clock_save_ss		resd	1
	asm_clock_save_ldt		resd	1
section .text:
global asm_clock_service
extern clock_processor
extern asm_interrupt_load_dataregs
	asm_clock_service:
		cli
		push 0
		int_save
		call asm_interrupt_load_dataregs

		mov dword[asm_clock_save_esp], esp
		mov word[asm_clock_save_ss], ss
		mov eax, dword[ss : esp + 4]	; Pointer To LDT
		mov dword[asm_clock_save_ldt], eax

		push asm_clock_save_esp
		push asm_clock_save_ss
		push asm_clock_save_ldt
		call clock_processor
		add esp, 12

		mov ebx, dword[asm_clock_save_esp]
		mov ecx, dword[asm_clock_save_ss]
		lldt [asm_clock_save_ldt]
		mov ss, cx
		mov esp, ebx

		push eax		; eax = return value (the return stack frame)
		int_restore
		add esp, 4
		sti
		iretd
