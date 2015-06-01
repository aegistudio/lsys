#include "define.h"
#include "interrupt.h"

void kernel_interrupt_setup()
{
	interrupt_controller_initialize();
}
