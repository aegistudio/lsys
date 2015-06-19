#include "define.h"
#include "segmentation.h"
#include "driver/interrupt.h"
#include "driver/keyboard.h"
#include "driver/video.h"

__public void kernel_interrupt_setup(selector* cs)
{
	interrupt_controller_initialize();
	interrupt_idt_initialize();
}

/*
byte input_buffer[video_line_length * video_line_count];
byte input_buffer_image[video_line_length * video_line_count];
dword pointer;
dword color_now;
byte is_insert_on;

void keyboard_event(word scancode, dword is_down)
{
	if(is_down)
	{
		int i;
		switch(scancode)
		{
			case keyboard_f1:
			case keyboard_f2:
			case keyboard_f3:
			case keyboard_f4:
			case keyboard_f5:
				color_now = scancode - keyboard_f1 + 0x07;
			break;
			case keyboard_leftarrow:
				if(pointer > 0) pointer --;
			break;
			case keyboard_rightarrow:
				if(input_buffer[pointer] != '\0') pointer ++;
			break;
			case keyboard_insert:
				is_insert_on = !is_insert_on;
			break;
			case keyboard_delete:
				i = pointer;
				while(input_buffer[i] != '\0')
				{
					input_buffer[i] = input_buffer[i + 1];
					i ++;
				}
			break;
			default:
			break;
		}
	}
}

void input_event(byte ascii)
{
	if(ascii == '\b')
	{
		if(pointer <= 0) return;
		pointer --;
		int i = pointer;
		while(input_buffer[i] != '\0')
		{
			input_buffer[i] = input_buffer[i + 1];
			i ++;
		}
		return;
	}
	else if((keyboard_keystate(keyboard_lshift) | keyboard_keystate(keyboard_lshift)) \
	 & keyboard_keystate(keyboard_tab))
	{
		if(ascii == '\t') return;
		if(ascii == 'Q') ascii = 'W';
		else if(ascii == 'W') ascii = 'E';
		else if(ascii == 'A') ascii = 'S';
		else if(ascii == 'S') ascii = 'D';
		else if(ascii == 'Z') ascii = 'X';
		else if(ascii == 'X') ascii = 'C';

		insert_at_pointer(ascii);
	}
	else
	{
		insert_at_pointer(ascii);
	}
}

void insert_at_pointer(byte current)
{
	if(is_insert_on)
	{
		dword i = pointer;
		for(; input_buffer[i] != 0; i ++);
		input_buffer[i + 1] = 0;
		for(; i > pointer; i --) input_buffer[i] = input_buffer[i - 1];
	}
	input_buffer[pointer] = current;
	pointer ++;
}

dword update_counter;
__public interrupt_stack_frame* clock_handler(interrupt_stack_frame* stack_frame)
{
	if((update_counter & 0x04) != 0)
	{
		video_clear_screen();
		video_put_string(input_buffer, color_now);
		if((update_counter & 0x10) != 0)
		{
			int i = 0;
			for(; i < pointer; i ++)
				input_buffer_image[i] = input_buffer[i];
			input_buffer_image[pointer] = '_';
			input_buffer_image[pointer + 1] = 0;
			video_set_cursor(0, 0);
			video_put_string(input_buffer_image, color_now);
		}
	}
	update_counter ++;

	if(update_counter == 0x0fff)
	{
		int i = 0;
		for(; i < sizeof(input_buffer); i ++)
			input_buffer[i] = 0;
		pointer = 0;
		update_counter = 0;
	}
	return stack_frame;
}

__public void kernel_interrupt_service()
{
	clock_initialize(clock_handler);
	keyboard_initalize(keyboard_event, input_event);
	int i = 0;
	for(;i < sizeof(input_buffer); i ++)
		input_buffer[i] = 0;
	pointer = 0;
	color_now = 0x07;
	is_insert_on = 1;
}

*/

#include "process.h"
#include "systemcall.h"
__public void kernel_interrupt_service()
{
	clock_initialize(scheduler_schedule);
	systemcall_stdout_initialize();
	systemcall_process_initialize();
}
