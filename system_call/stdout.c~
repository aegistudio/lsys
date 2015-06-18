#ifndef __stdout_c__
#define __stdout_c__

#include "define.h"
#include "api/system_call.h"
#include "driver/interrupt.h"

extern asm_system_call_stdout(interrupt_stack_frame*);

__public void system_call_stdout_init()
{
	interrupt_set_interrupt_handler(system_call_stdout, asm_system_call_stdout);
}

#endif
