#ifndef __stdout_c__
#define __stdout_c__

#define __systemcall_impl__

#include "define.h"
#include "systemcall.h"
#include "driver/interrupt.h"

extern asm_systemcall_stdout(interrupt_stack_frame*);

__public void systemcall_stdout_initialize()
{
	interrupt_set_interrupt_handler(systemcall_stdout, asm_systemcall_stdout);
}

char conversion_buffer[32];
__public void systemcall_stdout_putinteger(int integer, dword color, void (*systemcall_putstring)(byte*, dword))
{
	if(integer == 0) systemcall_putstring("0", color);
	else
	{
		if(integer < 0)
		{
			systemcall_putstring("-", color);
			integer = - integer;
		}

		conversion_buffer[31] = 0;
		int index = 31;
		
		while(integer > 0)
		{
			index --;
			conversion_buffer[index] = integer % 10 + '0';
			integer = integer / 10;
		}
		systemcall_putstring(conversion_buffer + index, color);
	}
}

#endif
