#ifndef __semaphore_h__
#define __semaphore_h__

#include "driver/interrupt.h"

#ifdef __semaphore_c__
	#define __semaphore_export __public
#else
	#define __semaphore_export extern
#endif

typedef struct __semaphore_t
{
	signed int semaphore;
	word waiting_queue_head;
	word waiting_queue_tail;
}
semaphore;

__semaphore_export interrupt_stack_frame* semaphore_init(int sem_id, int init_value, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame);
__semaphore_export interrupt_stack_frame* semaphore_p(int sem_id, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame);
__semaphore_export interrupt_stack_frame* semaphore_v(int sem_id, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame);

#endif
