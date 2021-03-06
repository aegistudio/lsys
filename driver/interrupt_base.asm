section .text
global asm_interrupt_outb
;asm_interrupt_outb(dword port, byte word)
asm_interrupt_outb:
	mov edx, dword[esp + 4]
	mov eax, dword[esp + 8]
	out dx, al
	nop
	nop
	nop
	nop
	ret

global asm_interrupt_inb
;asm_interrupt_inb(word port)
asm_interrupt_inb:
	mov edx, dword[esp + 4]
	mov eax, 0
	in al, dx
	nop
	nop
	nop
	nop
	ret

global interrupt_preserved_ds
global interrupt_preserved_es
global interrupt_preserved_fs
	interrupt_preserved_ds		dd	0
	interrupt_preserved_es		dd	0
	interrupt_preserved_fs		dd	0

global asm_interrupt_save_dataregs
asm_interrupt_save_dataregs:
	push eax
	mov eax, 0

	mov eax, ds
	mov dword[interrupt_preserved_ds], eax

	mov eax, es
	mov dword[interrupt_preserved_es], eax

	mov eax, fs
	mov dword[interrupt_preserved_fs], eax
	pop eax
	ret

global asm_interrupt_load_dataregs
asm_interrupt_load_dataregs:
	push eax
	mov eax, dword[cs : interrupt_preserved_ds]
	mov ds, ax
	mov eax, dword[cs : interrupt_preserved_es]
	mov es, ax
	mov eax, dword[cs : interrupt_preserved_fs]
	mov fs, ax
	pop eax
	ret

global asm_interrupt_get_cs
asm_interrupt_get_cs:
	mov eax, 0
	mov eax, cs
	ret

global asm_interrupt_set_idt
asm_interrupt_set_idt:
	push eax
	mov eax, dword[ss : esp + 8]
	lidt [eax]
	pop eax
	ret

global exception_tag_0_division_error
global exception_tag_1_debug
global exception_tag_2_nmi
global exception_tag_3_breakpoint
global exception_tag_4_overflow
global exception_tag_5_bounds_check
global exception_tag_6_invalid_opcode
global exception_tag_7_coprocessor_unavailable
global exception_tag_8_double_fault
global exception_tag_9_coprocessor_exceed
global exception_tag_10_invalid_tss
global exception_tag_11_not_present
global exception_tag_12_stack_fault
global exception_tag_13_general_protection
global exception_tag_14_page_fault
global exception_tag_15_reserved
global exception_tag_16_fpu_fault
global exception_tag_17_alignment_check
global exception_tag_18_machine_check
global exception_tag_19_simd_exception

extern exception_handler_bus

%include "include/interrupt.inc"

exception_tag_0_division_error:
	push 0xffffffff
	int_save
	push 0
	jmp exception_tag_bus
exception_tag_1_debug:
	push 0xffffffff
	int_save
	push 1
	jmp exception_tag_bus
exception_tag_2_nmi:
	push 0xffffffff
	int_save
	push 2
	jmp exception_tag_bus
exception_tag_3_breakpoint:
	push 0xffffffff
	int_save
	push 3
	jmp exception_tag_bus
exception_tag_4_overflow:
	push 0xffffffff
	int_save
	push 4
	jmp exception_tag_bus
exception_tag_5_bounds_check:
	push 0xffffffff
	int_save
	push 5
	jmp exception_tag_bus
exception_tag_6_invalid_opcode:
	push 0xffffffff
	int_save
	push 6
	jmp exception_tag_bus
exception_tag_7_coprocessor_unavailable:
	push 0xffffffff
	int_save
	push 7
	jmp exception_tag_bus
exception_tag_8_double_fault:
	push 0
	int_save
	push 8
	jmp exception_tag_bus
exception_tag_9_coprocessor_exceed:
	push 0xffffffff
	int_save
	push 9
	jmp exception_tag_bus
exception_tag_10_invalid_tss:
	int_save
	push 10
	jmp exception_tag_bus
exception_tag_11_not_present:
	int_save
	push 11
	jmp exception_tag_bus
exception_tag_12_stack_fault:
	int_save
	push 12
	jmp exception_tag_bus
exception_tag_13_general_protection:
	int_save
	push 13
	jmp exception_tag_bus
exception_tag_14_page_fault:
	int_save
	push 14
	jmp exception_tag_bus
exception_tag_15_reserved:
	int_save
	push 15
	jmp exception_tag_bus
exception_tag_16_fpu_fault:
	push 0xffffffff
	int_save
	push 16
	jmp exception_tag_bus
exception_tag_17_alignment_check:
	int_save
	push 17
	jmp exception_tag_bus
exception_tag_18_machine_check:
	push 0xffffffff
	int_save
	push 18
	jmp exception_tag_bus
exception_tag_19_simd_exception:
	push 0xffffffff
	int_save
	push 19
	jmp exception_tag_bus
exception_tag_bus:
	call asm_interrupt_load_dataregs
	call exception_handler_bus
	add esp, 4
	int_restore
	add esp, 4
	iretd

global asm_default_interrupt_handler
extern default_interrupt_handler
asm_default_interrupt_handler:
	push 0
	int_save
	call asm_interrupt_load_dataregs
	call default_interrupt_handler
	int_restore
	add esp, 4
	iretd
