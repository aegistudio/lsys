#include "define.h"
#include "segmentation.h"
#include "interrupt.h"

dt_pointer idt_pointer;
gate idt[0x0100];

dt_pointer* kernel_interrupt_setup(selector* code)
{
	interrupt_controller_initialize();
	idt_pointer.base = idt;
	idt_pointer.limit = sizeof(idt) - 1;
	return &idt_pointer;
}
