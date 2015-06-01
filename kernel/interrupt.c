#include "define.h"
#include "segmentation.h"
#include "interrupt.h"
#include "keyboard.h"
#include "video.h"

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

byte input_buffer[video_line_length * video_line_count];
dword pointer;
dword color_now;

void keyboard_event(word scancode, dword is_down)
{
	if(scancode == keyboard_backspace)
	{
		if(!is_down) return;
		if(pointer <= 0) return;
		pointer --;
		input_buffer[pointer] = 0;
		return;
	}
	if(scancode >= keyboard_f1 && scancode <= keyboard_f5)
	{
		if(!is_down) return;
		color_now = scancode - keyboard_f1 + 0x07;
		return;
	}
}

void input_event(byte ascii)
{
	if((keyboard_keystate(keyboard_lshift) | keyboard_keystate(keyboard_lshift)) \
	 & keyboard_keystate(keyboard_tab))
	{
		//do nothing
	}
	else
	{
		input_buffer[pointer] = ascii;
		pointer ++;
	}
}

__public void kernel_interrupt_service()
{
	interrupt_controller_set(interrupt_ir0_clock, 1);
	interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir0_clock, asm_interrupt_svc_clock);

	//interrupt_controller_set(interrupt_ir1_keyboard, 1);
	//interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir1_keyboard, asm_interrupt_svc_keyboard);
	keyboard_initalize(keyboard_event, input_event);
	int i = 0;
	for(;i < sizeof(input_buffer); i ++) input_buffer[i] = 0;
	pointer = 0;
	color_now = 0x07;
}

dword update_counter;
__public void kernel_interrupt_svc_clock()
{
	if((update_counter & 0x04) != 0)
	{
		video_clear_screen();
		video_put_string(input_buffer, color_now);
		if((update_counter & 0x08) != 0) video_put_char('_', color_now);
	}
	update_counter ++;

	if(update_counter == 0x0fff)
	{
		int i = 0;
		for(; i < sizeof(input_buffer); i ++) input_buffer[i] = 0;
		pointer = 0;
		update_counter = 0;
	}
	interrupt_controller_end(interrupt_ir0_clock);
}

//@Deprecated: Used For Test And Now Abandoned.
__public void kernel_interrupt_svc_keyboard(byte scancode)
{
	video_put_char(scancode, 0x07);
	interrupt_controller_end(interrupt_ir1_keyboard);
}
