#ifndef __clock_c__
#define __clock_c__

#include "driver/interrupt.h"
#include "driver/clock.h"

__private clock_handler handler;

__public interrupt_stack_frame* clock_processor(interrupt_stack_frame* stack_frame)
{
	stack_frame = handler(stack_frame);
	interrupt_controller_end(interrupt_ir0_clock);
	return stack_frame;
}

extern void asm_clock_service(interrupt_stack_frame* stack_frame);

__public void clock_initialize(clock_handler the_handler)
{
	handler = the_handler;
	interrupt_controller_set(interrupt_ir0_clock, 1);
	interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir0_clock, asm_clock_service);
}

#endif
