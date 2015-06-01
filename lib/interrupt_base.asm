section .text
global asm_interrupt_outb
;asm_interrupt_outb(word port, byte word)
asm_interrupt_outb:
	mov eax, dword[esp + 4]
	mov edx, dword[esp + 8]
	out dx, al
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
	ret
