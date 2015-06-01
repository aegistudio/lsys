section .text

global asm_interrupt_svc_clock
extern kernel_interrupt_svc_clock
asm_interrupt_svc_clock:
	call kernel_interrupt_svc_clock
	iretd

global asm_interrupt_svc_keyboard
extern kernel_interrupt_svc_keyboard
asm_interrupt_svc_keyboard:
	mov eax, 0
	in al, 0x60
	push eax
	call kernel_interrupt_svc_keyboard
	add esp, 4
	iretd
