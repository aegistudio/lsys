;********************************************************************
;			16 Bit Real Mode
; @Description: Now inside 16 bit real Mode Of Loader. Addressing uses
; 20 bit address line and segment registers. Now what we need to do is
; loading kernel into 1000h:0000h, setup descriptors and page tables
; then translate to protected mode.
;********************************************************************
loader.base 	equ 9000h
loader.offset	equ 0100h
kernel.base	equ 1000h
kernel.offset	equ 0000h
kernel.code.base equ 3000h
kernel.code.offset equ 0400h
org 0100h
jmp loaderCode16
nop

%include "boot/fat12header.inc"
%include "protect.inc"

loaderCode16:
	; Setup Registers
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov bp, loader.offset
	mov sp, bp

	; Clear Screen
	mov ax, 0600h
	mov bx, 0700h
	mov cx, 0000h
	mov dx, 0184fh
	int 10h

	; Print Now Inside Loader
	mov ax, nowInsideLoader
	mov bx, nowInsideLoader.length
	call loader.displayString16

	;Print Finding Kernel
	mov ax, findingKernel
	mov bx, findingKernel.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Finding kernel.elf In Root Directory
	push es
	mov ax, kernel.base
	mov es, ax

	mov dx, kernel.elf
	mov ax, rootEntry.areaSize + rootEntry.areaBeginSector
	mov cx, rootEntry.areaBeginSector
	mov bx, kernel.offset
	call floppy.findDirectory
	cmp ax, 0
	jne kernel.findSuccess

	; kernel.elf Not Found
	pop es
	mov ax, fail
	mov bx, fail.length
	call loader.writeToEnd
	jmp $

	; kernel.elf Found
	kernel.findSuccess:
	mov dx, es
	pop es
	push ax

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	; Display Loading Kernel
	mov ax, loadingKernel
	mov bx, loadingKernel.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Loading Kernel
	pop ax
	push es
	mov es, dx
	mov bx, kernel.offset

	call floppy.readFile

	mov dx, es
	pop es

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	mov ax, killingMotor
	mov bx, killingMotor.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	call floppy.killMotor

	mov ax, ok
	mov bx, ok.length
	call loader.writeToEnd

	mov ax, jumpingToProtectedMode
	mov bx, jumpingToProtectedMode.length
	call loader.displayString16

	mov ax, progress
	mov bx, progress.length
	call loader.writeToEnd

	; Setting Up Protected Mode

	; Load Global Descriptor Table Pointer
	lgdt [gdt.register]

	; Open A20 Address Line
	cli
	in al, 92h
	or al, 00000010b
	out 92h, al

	; Set Control Register 0 : Protected = 1.
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp dword selector.flat_code : loaderCode32

loader.displayString16:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 1301h
	mov bx, 000fh
	mov dx, word[loader.displayString.cursor]
	push dx
	int 10h
	pop dx
	mov dx, word[loader.displayString.lineLength]
	add word[loader.displayString.cursor], dx
	pop bp
	popa
	ret
	loader.displayString.cursor dw 0000h
	loader.displayString.lineLength dw 0100h

loader.writeToEnd:
	pusha
	push bp
	mov bp, ax
	mov cx, bx
	mov ax, 01301h
	mov dx, word[loader.displayString.cursor]
	sub dx, 0100h
	mov bx, 0050h
	sub bx, cx
	or dx, bx
	mov bx, 0007
	int 10h
	pop bp
	popa
	ret

%include "boot/floppy.inc"

kernel.elf db "KERNEL  ELF"

nowInsideLoader db "===================================LSYS 0.01===================================="
nowInsideLoader.length equ $ - nowInsideLoader

findingKernel db "Finding kernel..."
findingKernel.length equ $ - findingKernel

loadingKernel db "Loading kernel..."
loadingKernel.length equ $ - loadingKernel

killingMotor db "Killing floppy motor..."
killingMotor.length equ $ - killingMotor

jumpingToProtectedMode db "Jumping to protected mode..."
jumpingToProtectedMode.length equ $ - jumpingToProtectedMode

progress db "      [    ]"
progress.length equ $ - progress
ok db "      [ OK ]"
ok.length equ $ - ok
fail db "      [FAIL]"
fail.length equ $ - fail

; Defining Descriptors
; Label		Type		Base	Limit	Attributes	Privilege
gdt.base	descriptor	0,	0,	0,		privilege.kernel
gdt.random_data descriptor	0,	0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.flat_code	descriptor	loader.physic_address, 0fffffh,		\
	descriptor.code32 | descriptor.code.readable |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.flat_data	descriptor	loader.physic_address,	0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.video	descriptor	0b8000h,	0fffffh,	\
	descriptor.data16 | descriptor.data.readwrite |\
	descriptor.gran.byte | descriptor.present,\
	privilege.user
gdt.stack	descriptor	loader.physic_address,	00ffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.putchar32.offset	equ	loader.putchar32 - $$
gdt.putchar32	gate selector.flat_code, gdt.putchar32.offset, 0,	\
	descriptor.system.386 | descriptor.gate.call | descriptor.present,\
	privilege.kernel
gdt.kernel.file	descriptor	kernel.physic_address, 0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.kernel.memory	descriptor	0, 0fffffh,	\
	descriptor.data32 | descriptor.data.readwrite |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.kernel.code		descriptor	kernel.code.physic_address, 0fffffh,	\
	descriptor.code32 | descriptor.code.readable |\
	descriptor.gran.4kb | descriptor.present,\
	privilege.kernel
gdt.end:

gdt.register:
gdt.length	equ	gdt.end - gdt.base
	dw	gdt.length - 1
	dd	gdt.base + loader.physic_address

loader.physic_address equ loader.base * 10h
kernel.physic_address equ kernel.base * 10h
kernel.code.physic_address equ kernel.code.base * 10h

selector.kernel.file	equ	(gdt.kernel.file - gdt.base)|\
	selector.global | privilege.kernel
selector.kernel.memory	equ	(gdt.kernel.memory - gdt.base)|\
	selector.global | privilege.kernel
selector.kernel.code	equ	(gdt.kernel.code - gdt.base)|\
	selector.global | privilege.kernel
selector.random_data	equ	(gdt.random_data - gdt.base)|\
	selector.global | privilege.kernel
selector.flat_code	equ	(gdt.flat_code - gdt.base)|\
	selector.global | privilege.kernel
selector.flat_data	equ	(gdt.flat_data - gdt.base)|\
	selector.global | privilege.kernel
selector.video		equ	(gdt.video - gdt.base)|\
	selector.global | privilege.user
selector.stack		equ	(gdt.stack - gdt.base)|\
	selector.global | privilege.kernel
selector.putchar32	equ	(gdt.putchar32 - gdt.base)|\
	selector.global | privilege.kernel

;***********************************************************************
; *			32 Bit Protected Mode
; * @Description: Now inside 32 bit protected mode of loader, addressing
; * uses 32bit virtual address and descriptors.
;***********************************************************************
bits 32
align 32
loaderCode32:
	xor eax, eax
	mov eax, selector.flat_data
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov eax, selector.video
	mov gs, ax
	mov eax, selector.stack
	mov ss, ax
	mov ebp, loader.offset
	mov esp, ebp

	mov eax, ok
	mov ebx, ok.length
	call loader.writeToEnd32

	mov eax, settingUpPaging
	mov ebx, settingUpPaging.length
	call loader.displayString32

	mov eax, skip
	mov ebx, skip.length
	call loader.writeToEnd32

	mov eax, reallocatingKernel
	mov ebx, reallocatingKernel.length
	call loader.displayString32

	mov eax, progress
	mov ebx, progress.length
	call loader.writeToEnd32

	; Begin Analysing Of Kernel ELF File.
	mov eax, selector.kernel.file
	mov es, ax
	mov eax, selector.kernel.memory
	mov fs, ax

	; Check Whether The Magic Number Of The ELF Is Valid.
	mov eax, dword[es : (elf.ident.magic - elf.header.base)]
	mov ebx, dword[ds : elf.ident.magic.answer]
	cmp eax, ebx
	jne loader.kernel_relocate.fail

	; Interprete Program Header
	mov eax, dword[es : (elf.program_header.offset - elf.header.base)]
	mov dword[elf.program_header.offset], eax

	mov eax, dword[es : (elf.program_header.entry.count - elf.header.base)]
	and eax, 0000ffffh
	mov dword[elf.program_header.entry.count], eax

	; To Simplify We Only Have To Do Entry With PT_LOAD
	loader.program_header.interprete:
	mov eax, dword[ds : elf.program_header.entry.count]
	mov ebx, dword[ds : loader.program_header.current_entry]
	cmp ebx, eax
	jge loader.program_header.interprete.end

	mov eax, ebx
	mov ebx, dword[es : (elf.program_header.offset - elf.header.base)]
	mov ecx, dword[es : (elf.program_header.entry.size - elf.header.base)]
	and ecx, 0000ffffh
	mul ecx
	add ebx, eax
	and ebx, 0000ffffh
	mov eax, dword[es : bx]

	cmp eax, elf.program_header.type.load
	jne loader.program_header.not_pt_load

	; Do PT_LOAD Program Header
		mov eax, dword[es : bx + (elf.program_header.virtual_address - elf.program_header.base)]
		push ebx
		mov ebx, gdt.kernel.memory
		call descriptor.set_base
		pop ebx
		mov eax, selector.kernel.memory
		mov fs, ax

		; Clean Memory According To Memory_Size
		mov eax, dword[es : bx + (elf.program_header.memory_size - elf.program_header.base)]
		mov dword[ds : elf.program_header.memory_size], eax

		push ebx
		mov ebx, 0
		loader.program_header.clean_memory:
		mov eax, dword[ds : elf.program_header.memory_size]
		cmp ebx, eax
		jge loader.program_header.clean_memory.end
		mov dword[fs : ebx], 0
		add ebx, 4
		jmp loader.program_header.clean_memory
		loader.program_header.clean_memory.end:
		pop ebx

		; Copy Memory According To File_Size
		mov eax, dword[es : bx + (elf.program_header.file_size - elf.program_header.base)]
		mov dword[ds : elf.program_header.file_size], eax

		mov eax, dword[es : bx + (elf.program_header.memory_offset - elf.program_header.base)]
		mov dword[ds : elf.program_header.memory_offset], eax

		push ebx
		loader.program_header.copy_file:
		mov eax, dword[ds : elf.program_header.file_size]
		mov ebx, dword[ds : loader.program_header.current_counter]
		cmp ebx, eax
		jge loader.program_header.copy_file.end
		mov eax, dword[ds : elf.program_header.memory_offset]
		add ebx, eax
		mov eax, dword[es : ebx]
		mov ebx, dword[ds : loader.program_header.current_counter]
		mov dword[fs : ebx], eax
		add dword[ds : loader.program_header.current_counter], 4
		jmp loader.program_header.copy_file
		loader.program_header.copy_file.end:
		pop ebx		

	loader.program_header.not_pt_load:
	inc dword[loader.program_header.current_entry]
	jmp loader.program_header.interprete
	loader.program_header.interprete.end:

	jmp loader.kernel_relocate.success

	loader.program_header.current_entry dd 0
	loader.program_header.current_counter dd 0

	loader.kernel_relocate.fail:
	mov eax, selector.flat_data
	mov es, ax
	mov fs, ax
	mov eax, fail
	mov ebx, fail.length
	call loader.writeToEnd32

	hlt

	loader.kernel_relocate.success:
	mov eax, selector.flat_data
	mov es, ax
	mov fs, ax
	mov eax, ok
	mov ebx, ok.length
	call loader.writeToEnd32

	mov eax, launchingKernel
	mov ebx, launchingKernel.length
	call loader.displayString32

	mov eax, progress
	mov ebx, progress.length
	call loader.writeToEnd32

	; Random Access Will Be Required In Kernel, So Setup Random Access
	mov ax, selector.random_data
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	jmp selector.kernel.code : kernel.code.offset

abcdefg db "ABCDEFG"
length equ $ - abcdefg

loader.putchar32:
	; Parameter:
	; 	dx = The Character To Print
	;	ebx = The Position Of The Word On Buffer
	push eax
	shl ebx, 1
	mov eax, dword[gs : ebx]
	mov ah, dh
	mov al, dl
	mov dword[gs : ebx], eax
	inc ebx
	pop eax
	ret

loader.print32:
	loader.print32.string equ 4d
	loader.print32.limit equ 8d
	loader.print32.color equ 12d
	loader.print32.position equ 16d
	loader.print32.stacklimit equ 16d
	; Parameter:
	; 	ds : eax = The Address Of The String
	;	ebx = The Length Of The String
	;	cl = The Color Of The String
	;	edx = The Position Of The String
	pusha
	push ebp
	mov ebp, esp
	sub esp, loader.print32.stacklimit

	mov dword[ss : (ebp - loader.print32.string)], eax
	add ebx, eax
	mov dword[ss : (ebp - loader.print32.limit)], ebx
	mov byte[ss : (ebp - loader.print32.color)], cl
	mov dword[ss : (ebp - loader.print32.position)], edx

	loader.print32.print:
	mov eax, dword[ss : (ebp - loader.print32.string)]
	mov ebx, dword[ss : (ebp - loader.print32.limit)]
	cmp eax, ebx
	jge loader.print32.endprint
	mov dl, byte[ds : eax]
	mov dh, byte[ss : (ebp - loader.print32.color)]
	mov ebx, dword[ss : (ebp - loader.print32.position)]
	call loader.putchar32
	inc dword[ss : (ebp - loader.print32.string)]
	inc dword[ss : (ebp - loader.print32.position)]
	jmp loader.print32.print

	loader.print32.endprint
	add esp, loader.print32.stacklimit
	pop ebp
	popa
	ret

loader.displayString32:
	pusha
	mov edx, dword[ds : loader.displayString.cursor]
	shr edx, 2
	and edx, 0ffffh
	add edx, 0050h
	mov cl, 0fh
	call loader.print32
	add word[ds : loader.displayString.cursor], 0140h
	popa
	ret

loader.writeToEnd32:
	pusha
	mov cl, 07h
	mov edx, dword[ds : loader.displayString.cursor]
	shr edx, 2
	and edx, 0ffffh
	sub edx, ebx
	add edx, 0050h
	call loader.print32
	popa
	ret

skip db "     [SKIP]"
skip.length equ $ - skip

settingUpPaging db "Setting up paging..."
settingUpPaging.length equ $ - settingUpPaging

reallocatingKernel db "Reallocating kernel..."
reallocatingKernel.length equ $ - reallocatingKernel

launchingKernel db "Launching kernel..."
launchingKernel.length equ $ - launchingKernel

%include "elf.inc"
