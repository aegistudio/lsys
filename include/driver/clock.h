#ifndef __clock_h__
#define __clock_h__

#include "define.h"
#include "driver/interrupt.h"

#ifdef __clock_c__
	#define __clock_h_export __public
#else
	#define __clock_h_export extern
#endif

typedef interrupt_stack_frame* (*clock_handler)(selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame);

__clock_h_export void clock_initialize(clock_handler handler);

#endif
