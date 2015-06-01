#include "define.h"
#include "segmentation.h"
#include "interrupt.h"

dt_pointer idt_pointer;
gate idt[0x0100];

__public dt_pointer* kernel_interrupt_setup(selector* cs)
{
	interrupt_controller_initialize();
	idt_pointer.base = idt;
	idt_pointer.limit = sizeof(idt) - 1;
	interrupt_idt_set_pointer(&idt_pointer, cs);
	return &idt_pointer;
}

extern void asm_interrupt_svc_clock();
__public void kernel_interrupt_service()
{
	interrupt_controller_set(interrupt_ir0_clock, 1);
	interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir0_clock, asm_interrupt_svc_clock);
}

#include "video.h"
__public void kernel_interrupt_svc_clock()
{
	video_put_char('#', 0x07);
	interrupt_controller_end(interrupt_ir0_clock);
}
