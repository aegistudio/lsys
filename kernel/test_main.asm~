global test_main_stdldt
global test_main
global end_of_test_main
global test_main_data
global end_of_test_main_data
global test_main_pcb

extern video_put_char

section .data
	test_main_data:
	runtime_stack	resd	60
	test_main_stack_base dd $ - runtime_stack
	test_main_pcb:
		times 40 dd 0
	test_main_stdldt:
		times 40 dd 0
	end_of_test_main_data	dd $ - test_main_data

section .text
	test_main:
	push 0x72
	push 0x40
	call video_put_char
	add esp, 8
	jmp test_main
	end_of_test_main	dd	$ - test_main
