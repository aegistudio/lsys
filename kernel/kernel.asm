global kernel_main
extern kernel_gdt_migration
extern kernel_video_setup
extern kernel_interrupt_setup
extern kernel_interrupt_service

section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

old_gdt_pointer		resd	6

section .text
kernel_main:
	; Resetting Stack And Global Descriptor Table.
	mov esp, kernel.stack_base

	sgdt [old_gdt_pointer]
	push old_gdt_pointer
	call kernel_gdt_migration
	lgdt [eax]

	; Setup Kernel Screen Environment
	call kernel_video_setup

	; Setup Interrupt Controllers And IDT.
	push cs
	call kernel_interrupt_setup
	lidt [eax]

	; Enable Some Interrupt And Set Their Routines
	call kernel_interrupt_service
	sti

	jmp $
