global test_main_stdldt
global test_main
global end_of_test_main
global test_main_data
global end_of_test_main_data
global runtime_stack
global runtime_kernel_stack
global test_main_stack_limit
global test_main_kernel_stack_limit
global asm_test_main_init

extern video_put_char

section .bss
	test_main_data:
	runtime_stack	resd	60
	test_main_stack_limit equ $ - runtime_stack
	runtime_kernel_stack	resd	60
	test_main_kernel_stack_limit equ $ - runtime_kernel_stack
	test_main_stdldt resd 60
	end_of_test_main_data	equ $ - test_main_data
	

section .text
test_main_string	db		"Inside test main now!", 0
extern test_main_init
	test_main:
	mov eax, 1
	mov esi, 0x0002
	mov edi, test_main_string
	int 21h
	jmp test_main
	end_of_test_main	equ	$ - test_main

	asm_test_main_init:
		pushf
		push 0fffffh
		push 0b8000h
		push test_main_kernel_stack_limit
		push runtime_kernel_stack
		push test_main_stack_limit
		push runtime_stack
		push end_of_test_main_data
		push test_main_data
		push end_of_test_main
		push test_main
		push test_main_stdldt
		call test_main_init
		add esp, 48
		ret
