#ifndef __stdout_c__
#define __stdout_c__

#define __systemcall_impl__

#include "define.h"
#include "systemcall.h"
#include "driver/interrupt.h"

extern asm_systemcall_process(interrupt_stack_frame*);

__public void systemcall_process_initialize()
{
	interrupt_set_interrupt_handler(systemcall_process, asm_systemcall_process);
}

#endif
