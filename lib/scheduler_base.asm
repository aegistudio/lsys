global scheduler_cs
global scheduler_ds
global scheduler_es
global scheduler_fs
global scheduler_ss
global scheduler_gs

section .bss
	scheduler_ds	resb 4
	scheduler_es	resb 4
	scheduler_fs	resb 4
	scheduler_ss	resb 4
	scheduler_gs	resb 4
	scheduler_cs	resb 4
	scheduler_temp_stack_frame resd	40

extern gdt_pointer
section .text
global asm_scheduler_get_gdt
asm_scheduler_get_gdt:
	sgdt [gdt_pointer]
	ret

global asm_scheduler_set_gdt
asm_scheduler_set_gdt:
	lgdt [gdt_pointer]
	ret

global asm_scheduler_set_taskregs
asm_scheduler_set_taskregs:
	push eax
	mov eax, dword[ss : (esp + 8)]
	lldt ax
	mov eax, dword[ss : (esp + 12)]
	ltr ax
	pop eax;
	ret

global asm_scheduler_get_stackframe
global asm_scheduler_copy_stackframe
asm_scheduler_get_stackframe:
	mov eax, scheduler_temp_stack_frame
	ret
	
asm_scheduler_copy_stackframe:
	push ebp
	mov ebp, esp
	pusha
	push fs

	mov eax, dword[ss : ebp + 8]
	mov fs, ax			; FS = Destin Selector

	mov eax, dword[ss : ebp + 12]	; EAX = Begin
	mov edx, scheduler_temp_stack_frame

	mov dword[fs : eax], eax
	add eax, 4

	loop_copy_stack_frame:
	mov ebx, dword[ss : ebp + 16]	; EBX = End
	cmp eax, ebx
	jge end_copy_stack_frame
		mov cl, byte[ds : edx]
		mov byte[fs : eax], cl
		inc edx
		inc eax
	jmp loop_copy_stack_frame
	end_copy_stack_frame:
	pop fs
	popa
	mov esp, ebp
	pop ebp
	ret

global asm_scheduler_get_selectors
asm_scheduler_get_selectors:
	pusha

	mov eax, 0
	mov eax, cs
	mov dword[scheduler_cs], eax


	mov eax, 0
	mov eax, ds
	mov dword[scheduler_ds], eax


	mov eax, 0
	mov eax, ss
	mov dword[scheduler_ss], eax


	mov eax, 0
	mov eax, gs
	mov dword[scheduler_gs], eax


	mov eax, 0
	mov eax, fs
	mov dword[scheduler_fs], eax


	mov eax, 0
	mov eax, es
	mov dword[scheduler_es], eax

	popa
	ret

global asm_scheduler_set_selectors
asm_scheduler_set_selectors:
	pusha

	mov eax, dword[scheduler_ds]
	mov ds, eax

	mov eax, dword[scheduler_ss]
	mov ss, eax

	mov eax, dword[scheduler_gs]
	mov gs, eax

	mov eax, dword[scheduler_fs]
	mov fs, eax

	mov eax, dword[scheduler_es]
	mov es, eax

	push dword[scheduler_cs]
	push scheduler_transmit_instruction
	retf

	scheduler_transmit_instruction:

	popa
	ret
