section .text
global asm_keyboard_service
extern keyboard_processor
	asm_keyboard_service:
		mov eax, 0
		in al, 0x60
		push eax
		call keyboard_processor
		add esp, 4
		iretd
