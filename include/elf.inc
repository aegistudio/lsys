;****************************************************************************
; * 				elf.inc
; * @Title: Executable Linkable Format Assembly File Header
; * @Author: LuoHaoran
; * @Description: This file describes the general structure of elf file, and
; * some essentials for parsing and reading elf file.
;****************************************************************************

elf.header.base:
	elf.ident.magic			resb		4
	elf.ident.file_class		resb		1
	elf.ident.data_encoding		resb		1
	elf.ident.version		resb		1
	elf.ident.padding		resb		9
	elf.type			resw		1
	elf.machine			resw		1
	elf.version			resd		1
	elf.entry			resd		1
	elf.program_header.offset	resd		1
	elf.section_header.offset	resd		1
	elf.flags			resd		1
	elf.header.size			resw		1
	elf.program_header.entry.size	resw		1
	elf.program_header.entry.count	resw		1
	elf.section_header.entry.size	resw		1
	elf.section_header.entry.count	resw		1
	elf.section_header.string_table	resw		1
elf.header.end:

elf.machine.none	equ	0	; No Machine
elf.machine.m32		equ	1	; AT&T WE 32100
elf.machine.sparc	equ	2	; Sparc
elf.machine.386		equ	3	; Intel 80386
elf.machine.68k		equ	4	; Motorola 68000
elf.machine.88k		equ	5	; Motorola 88000
elf.machine.860		equ	7	; Intel 80860
elf.machine.mips	equ	8	; Mips RS3000

elf.version.none	equ	0	; Invalid Version
elf.version.current	equ	1	; Current Version

elf.ident.magic.answer	db	0x7f, 'E', 'L', 'F'
elf.ident.class.none	equ	0	; Invalid Class
elf.ident.class.32	equ	1	; 32-Bit Object
elf.ident.class.64	equ	2	; 64-Bit Object

elf.ident.data.none		equ	0	; Invalid Data Endian
elf.ident.data.smallendian	equ	1	; Small-Endian
elf.ident.data.bigendian	equ	2	; Big-Endian

elf.program_header.base:
	elf.program_header.type			resd	1
	elf.program_header.memory_offset	resd	1
	elf.program_header.virtual_address	resd	1
	elf.program_header.physic_address	resd	1
	elf.program_header.file_size		resd	1
	elf.program_header.memory_size		resd	1
	elf.program_header.flags		resd	1
	elf.program_header.align		resd	1
elf.program_header.end:

elf.program_header.type.null	equ	0
elf.program_header.type.load	equ	1
elf.program_header.type.dynamic	equ	2
elf.program_header.type.interp	equ	3
elf.program_header.type.note	equ	4
elf.program_header.type.shared	equ	5

