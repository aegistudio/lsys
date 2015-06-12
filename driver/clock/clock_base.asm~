%include "include/interrupt.inc"

section .text:
global asm_clock_service
extern clock_processor
	asm_clock_service:
		push 0
		int_save

		call clock_processor

		push eax		; eax = return value (the return stack frame)
		int_restore
		add esp, 4
		iretd
