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
		sldt [asm_clock_save_ldt]

		push asm_clock_save_esp
		push asm_clock_save_ss
		push asm_clock_save_ldt
		call clock_processor
		
		add esp, 12
		mov ebx, dword[asm_clock_save_esp]
		mov ecx, dword[asm_clock_save_ss]
		lldt [asm_clock_save_ldt]
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
