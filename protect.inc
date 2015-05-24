;********************************************************************
; *			protect.inc
; * @Title: protected mode critical assembly include file
; * @Author: LuoHaoran
; * @Description: This file declares some constants and marcos for
; * segment descriptors, gate descriptors and selectors.
;********************************************************************

privilege.system		equ	00h
privilege.kernel		equ	00h
privilege.root_service		equ	01h
privilege.service		equ	02h
privilege.user			equ	03h
privilege.application		equ	03h

;********************************************************************
; * @Name: selector(word address, byte attribute)
; * @Description: a selector is an index to descriptor which is stored
; * in segment registers in protection mode.
; * 	struct selector{
; *		byte attribute: 3
; *		word address: 15
; *	};
; * @Param address the address bias from the descriptor to baseline
; * of local descriptor table or global descriptor table.
; * @Param attribute the attributes for the selector
;*********************************************************************
selector.local 		equ		4
selector.global		equ		0
%macro selector 2
	dw	(%1 & 0fff8h) | %2
%endmacro

;********************************************************************
; * @Name: descriptor(word base, word limit, dword attribute, byte dpl)
; * @Description: the descriptor describes the segment which could be
; * used for address translation, privilege check and boundary check.
; * @Param base the base address of the descriptor
; * @Param limit the size of the descriptor (low 12 bits are available)
; * @Param attribute the attributes for the descriptor
; * @Param dpl the descriptor privilege level for the descriptor
;********************************************************************
descriptor.data32		equ	4010h
descriptor.data16		equ	0010h
descriptor.expdown4gb		equ	4014h
descriptor.expdown64k		equ	0014h
descriptor.stack32		equ	4010h
descriptor.stack16		equ	0010h
descriptor.code32		equ	4018h
descriptor.code16		equ	0018h
descriptor.system		equ	0000h

descriptor.accessed		equ	01h

descriptor.data.readonly	equ	00h
descriptor.data.readwrite	equ	02h

descriptor.code.noread		equ	00h
descriptor.code.readable	equ	02h
descriptor.code.conform		equ	04h

descriptor.system.286		equ	00h
descriptor.system.386		equ	08h

descriptor.system.tss		equ	01h
descriptor.system.busy		equ	02h
descriptor.system.idle		equ	00h

descriptor.system.ldt		equ	02h
descriptor.gate.call		equ	04h
descriptor.gate.interrupt	equ	06h
descriptor.gate.trap		equ	07h

descriptor.missing		equ	00h
descriptor.present		equ	80h

descriptor.gran.byte		equ	0000h
descriptor.gran.4kb		equ	8000h

descriptor.none			equ	0000h
descriptor.available		equ	1000h

%macro descriptor 4
	dw	%2 & 0ffffh		;b0~b1: limit[0:15]
	dw	%1 & 0ffffh		;b2~b3: base[0:15]
	db	(%1 >> 16) & 0ffh	;b4: base[16:23]
	db	%3 & 0ffh | (%4 & 03h) << 5
			;b5: privilege << 5 | attribute[0:7]
	db	((%3 >> 8) & 0f0h) | (%2 >> 16) & 0fh
			;b6: attribute[12:15] << 4 | limit[16:19]
	db	(%1>>24) & 0ffh		;b7: base[24:31]
%endmacro
