#ifndef __keyboard_c__
#define __keyboard_c__

#include "driver/keyboard.h"
#include "driver/interrupt.h"

__private byte keyboard_scancode_state[0xff];

__private byte keyboard_e0_scancode_state[0xff];
__private byte keyboard_e1_scancode_state[0xff];

__private byte keyboard_scancode_ascii[0xff];
__private byte keyboard_ascii_major[0xff];

#define __keyboard_registry(scancode, index, major) \
	keyboard_scancode_ascii[scancode] = index;	\
	keyboard_ascii_major[index] = major;

__private byte extension;
__private byte caps_lock;
__private byte shift;

keyboard_event_handler event_handler;
keyboard_input_handler input_handler;

extern void asm_keyboard_service();
__public void keyboard_initalize(keyboard_event_handler event, keyboard_input_handler input)
{
	event_handler = event;
	input_handler = input;

	int i = 0;
	for(; i < 0xff; i ++)
	{
	 	keyboard_scancode_ascii[i] = 0;
		keyboard_ascii_major[i] = 0;
		keyboard_scancode_state[i] = 0;
		keyboard_e0_scancode_state[i] = 0;
		keyboard_e1_scancode_state[i] = 0;
	}

	__keyboard_registry(keyboard_space, ' ', ' ');
	__keyboard_registry(keyboard_backspace, '\b', '\b');
	__keyboard_registry(keyboard_enter, '\n', '\n');
	__keyboard_registry(keyboard_tab, '\t', '\t');

	__keyboard_registry(keyboard_a, 'a', 'A');
	__keyboard_registry(keyboard_b, 'b', 'B');
	__keyboard_registry(keyboard_c, 'c', 'C');
	__keyboard_registry(keyboard_d, 'd', 'D');
	__keyboard_registry(keyboard_e, 'e', 'E');
	__keyboard_registry(keyboard_f, 'f', 'F');
	__keyboard_registry(keyboard_g, 'g', 'G');
	__keyboard_registry(keyboard_h, 'h', 'H');
	__keyboard_registry(keyboard_i, 'i', 'I');
	__keyboard_registry(keyboard_j, 'j', 'J');
	__keyboard_registry(keyboard_k, 'k', 'K');
	__keyboard_registry(keyboard_l, 'l', 'L');
	__keyboard_registry(keyboard_m, 'm', 'M');
	__keyboard_registry(keyboard_n, 'n', 'N');
	__keyboard_registry(keyboard_o, 'o', 'O');
	__keyboard_registry(keyboard_p, 'p', 'P');
	__keyboard_registry(keyboard_q, 'q', 'Q');
	__keyboard_registry(keyboard_r, 'r', 'R');
	__keyboard_registry(keyboard_s, 's', 'S');
	__keyboard_registry(keyboard_t, 't', 'T');
	__keyboard_registry(keyboard_u, 'u', 'U');
	__keyboard_registry(keyboard_v, 'v', 'V');
	__keyboard_registry(keyboard_w, 'w', 'W');
	__keyboard_registry(keyboard_x, 'x', 'X');
	__keyboard_registry(keyboard_y, 'y', 'Y');
	__keyboard_registry(keyboard_z, 'z', 'Z');

	__keyboard_registry(keyboard_0, '0', ')');
	__keyboard_registry(keyboard_1, '1', '!');
	__keyboard_registry(keyboard_2, '2', '@');
	__keyboard_registry(keyboard_3, '3', '#');
	__keyboard_registry(keyboard_4, '4', '$');
	__keyboard_registry(keyboard_5, '5', '%');
	__keyboard_registry(keyboard_6, '6', '^');
	__keyboard_registry(keyboard_7, '7', '&');
	__keyboard_registry(keyboard_8, '8', '*');
	__keyboard_registry(keyboard_9, '9', '(');

	__keyboard_registry(keyboard_semicolon, ';', ':');
	__keyboard_registry(keyboard_squarebrack_left, '[', '{');
	__keyboard_registry(keyboard_squarebrack_right, ']', '}');
	__keyboard_registry(keyboard_comma, ',', '<');
	__keyboard_registry(keyboard_dot, '.', '>');
	__keyboard_registry(keyboard_slash, '/', '?');
	__keyboard_registry(keyboard_backslash, '\\', '|');
	__keyboard_registry(keyboard_upper_dot, '`', '~');
	__keyboard_registry(keyboard_apostrophe, '\'', '\"');
	__keyboard_registry(keyboard_minus, '-', '_');
	__keyboard_registry(keyboard_equal, '=', '+');

	extension = 0;
	caps_lock = 0;
	shift = 0;

	interrupt_controller_set(interrupt_ir1_keyboard, 1);
	interrupt_set_interrupt_handler(interrupt_vector_base + interrupt_ir1_keyboard, asm_keyboard_service);
}

__public void keyboard_processor(byte input)
{
	if(input == keyboard_extension0 || input == keyboard_extension1)
	{
		extension = input;
	}
	else
	{
		byte is_down = 1;
		byte state_changes = 0;
		if(input >= 0x80)
		{
			is_down = 0;
			input = input - 0x80;
		}

		byte* query_pointer = &keyboard_scancode_state[input];
		if(extension == keyboard_extension0)
			query_pointer = &keyboard_e0_scancode_state[input];
		else if(extension == keyboard_extension1)
			query_pointer = &keyboard_e1_scancode_state[input];

		if(*query_pointer != is_down)
		{
			state_changes = 1;
			*query_pointer = is_down;
		}
		

		if(extension)
		{
			if(state_changes)
				event_handler(extension << 8 | input, is_down);
			extension = 0;
		}
		else
		{
			if(state_changes)
				event_handler(input, is_down);

			if(is_down)
			{
				byte ascii = keyboard_scancode_ascii[input];
				shift = keyboard_scancode_state[keyboard_lshift]
					| keyboard_scancode_state[keyboard_rshift];
				if(input == keyboard_caps_lock)
					caps_lock = !caps_lock;
				if(ascii != 0)
				{
					if(caps_lock)
					{
						if(!shift)
						{
							if(ascii >= 'a' && ascii <= 'z')
								input_handler(keyboard_ascii_major[ascii]);
							else input_handler(ascii);
						}
						else
						{
							if(ascii >= 'a' && ascii <= 'z')
								input_handler(ascii);
							else input_handler(keyboard_ascii_major[ascii]);
						}
					}
					else
					{
						if(shift) input_handler(keyboard_ascii_major[ascii]);
						else input_handler(ascii);
					}
				}
			}
		}
	}
	interrupt_controller_end(interrupt_ir1_keyboard);
}

byte keyboard_keystate(word scancode)
{
	byte* query_pointer = &keyboard_scancode_state[scancode & 0x007f];
	if((scancode >> 8) & 0x00ff == keyboard_extension0)
		query_pointer = &keyboard_e0_scancode_state[scancode & 0x007f];
	else if((scancode >> 8) & 0x00ff == keyboard_extension1)
		query_pointer = &keyboard_e1_scancode_state[scancode & 0x007f];
	return *query_pointer;
}
#endif
