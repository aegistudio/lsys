%include "include/interrupt.inc"

section .text:
global asm_clock_service
extern clock_processor
extern asm_interrupt_load_dataregs
	asm_clock_service:
		cli
		push 0
		int_save
		call asm_interrupt_load_dataregs
		call clock_processor		
		push eax		; eax = return value (the return stack frame)
		int_restore
		add esp, 4
		sti
		iretd
