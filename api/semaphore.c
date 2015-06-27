#ifndef __stdout_c__
#define __stdout_c__

#define __systemcall_impl__

#include "define.h"
#include "systemcall.h"
#include "driver/interrupt.h"

extern asm_systemcall_semaphore(interrupt_stack_frame*);

__public void systemcall_semaphore_initialize()
{
	interrupt_set_interrupt_handler(systemcall_semaphore, asm_systemcall_semaphore);
}

#endif
