global kernel_main
extern kernel_gdt_migration
extern kernel_video_setup
extern kernel_interrupt_setup
extern kernel_interrupt_service

extern video_put_char
extern asm_video_brush_screen
;extern asm_test_main_init
extern asm_barber_init

section .bss
kernel.stack		resd	1024 * 2
kernel.stack_base:

old_gdt_pointer		resd	6

section .text
kernel_main_test_string		db	"010010", 0
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

	sti
	;call asm_test_main_init
	call asm_barber_init

	jmp $
