section .text

global asm_interrupt_svc_clock
extern kernel_interrupt_svc_clock
asm_interrupt_svc_clock:
	call kernel_interrupt_svc_clock
	iretd
