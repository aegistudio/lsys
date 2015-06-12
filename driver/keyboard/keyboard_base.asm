section .text
global asm_keyboard_service
extern keyboard_processor
extern asm_interrupt_load_dataregs

	asm_keyboard_service:
		int_save
		call asm_interrupt_load_dataregs
		mov eax, 0
		in al, 0x60
		push eax
		call keyboard_processor
		add esp, 4
		int_restore
		iretd
