#ifndef __semaphore_c__
#define __semaphore_c__

#include "define.h"
#include "semaphore.h"
#include "process.h"

__private semaphore semaphore_list[0x00ff];

__public interrupt_stack_frame* semaphore_init(int sem_id, int init_value, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame)
{
	semaphore_list[sem_id].semaphore = init_value;
	semaphore_list[sem_id].waiting_queue_head = 0;
	semaphore_list[sem_id].waiting_queue_tail = 0;
}

#include "driver/video.h"
__public interrupt_stack_frame* semaphore_p(int sem_id, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame)
{
	semaphore_list[sem_id].semaphore --;
	if(semaphore_list[sem_id].semaphore < 0)
	{
		scheduler_wait(&(semaphore_list[sem_id].waiting_queue_head), &(semaphore_list[sem_id].waiting_queue_tail),
			ldt, ss, esp, stack_frame);
	}
}

__public interrupt_stack_frame* semaphore_v(int sem_id, selector* ldt, selector* ss, dword* esp, interrupt_stack_frame* stack_frame)
{
	semaphore_list[sem_id].semaphore ++;
	if(semaphore_list[sem_id].semaphore <= 0)
	{
		scheduler_invoke(&(semaphore_list[sem_id].waiting_queue_head), &(semaphore_list[sem_id].waiting_queue_tail),
			ldt, ss, esp, stack_frame);
	}
}

#endif


