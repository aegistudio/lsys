global kernel_main
section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

section .text
kernel_main:
	mov esp, kernel.stack_base
	jmp $
