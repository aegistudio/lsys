global kernel_main
extern kernel_gdt_migration
extern kernel_video_setup
extern kernel_interrupt_setup

section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

old_gdt_pointer		resw	6
new_gdt_pointer		resw	6
gdt_pointer_base	resw	0fffh * 8

section .text
kernel_main:
	; Resetting Stack And Global Descriptor Table.
	mov esp, kernel.stack_base

	mov dword[new_gdt_pointer + 2], gdt_pointer_base
	sgdt [old_gdt_pointer]
	push new_gdt_pointer
	push old_gdt_pointer
	call kernel_gdt_migration
	lgdt [new_gdt_pointer]

	; Setup Kernel Screen Environment
	call kernel_video_setup

	; Setup Interrupt Controllers And IDT.
	;call kernel_interrupt_setup

	jmp $
