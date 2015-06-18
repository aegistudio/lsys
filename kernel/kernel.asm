global kernel_main
extern kernel_gdt_migration
extern kernel_video_setup
extern kernel_interrupt_setup
extern kernel_interrupt_service

extern video_put_char
extern asm_video_brush_screen

section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

old_gdt_pointer		resd	6

section .text
kernel_main:
	; Resetting Stack And Global Descriptor Table.
	mov esp, kernel.stack_base

	call kernel_gdt_migration

	; Setup Kernel Screen Environment
	call kernel_video_setup

	; Setup Interrupt Controllers And IDT.
	mov eax, 0
	mov eax, cs
	push eax
	call kernel_interrupt_setup
	add esp, 4

	; Enable Some Interrupt And Set Their Routines
	call kernel_interrupt_service
	;sti

	print:
	push 0x02
	push 0x40
	call video_put_char
	add esp, 8

	push 0x0220
	push 0x0000
	push 0xe0004000
	push 0xffffffff
	call asm_video_brush_screen
	add esp, 16

	jmp print
