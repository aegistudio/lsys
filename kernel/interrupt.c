#include "define.h"
#include "segmentation.h"
#include "interrupt.h"
#include "keyboard.h"

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
extern void asm_interrupt_svc_keyboard();

void keyboard_event(word scancode, dword is_down)
{
	if(is_down) video_put_char('#', 0x7f);
	else video_put_char('*', 0x7f);
}

void input_event(byte ascii)
{
	
}

__public void kernel_interrupt_service()
{
	interrupt_controller_set(interrupt_ir0_clock, 1);
	interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir0_clock, asm_interrupt_svc_clock);

	//interrupt_controller_set(interrupt_ir1_keyboard, 1);
	//interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir1_keyboard, asm_interrupt_svc_keyboard);
	keyboard_initalize(keyboard_event, input_event);
}

#include "video.h"
byte color_clock = 0;
__public void kernel_interrupt_svc_clock()
{
//	video_put_char('#', color_clock);
	color_clock += 1;
	interrupt_controller_end(interrupt_ir0_clock);
}

__public void kernel_interrupt_svc_keyboard(byte scancode)
{
	video_put_char(scancode, 0x07);
	interrupt_controller_end(interrupt_ir1_keyboard);
}
