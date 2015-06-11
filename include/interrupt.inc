%ifndef __interrupt_inc__
%define __interrupt_inc__

	%macro int_save 0
		pushad
		push gs
		push fs
		push ds
		push es
		push esp
	%endmacro

	%macro int_restore 0
		pop esp
		pop es
		pop ds
		pop fs
		pop gs
		popad
	%endmacro

%endif
