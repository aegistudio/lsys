kernel.physic_address	equ	0x30400

global kernel_main
section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

section .text
kernel_main:
	mov esp, kernel.physic_address + kernel.stack_base
	jmp $
