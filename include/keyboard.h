#ifndef __keyboard_h__
#define __keyboard_h__

#include "define.h"

#define keyboard_escape		0x01
#define keyboard_tab		0x0f
#define keyboard_space		0x39
#define keyboard_enter		0x1c
#define keyboard_backspace	0x0e
#define keyboard_num_lock	0x45
#define keyboard_scroll_lock	0x46
#define keyboard_caps_lock	0x3a

#define keyboard_a		0x1e
#define keyboard_b		0x30
#define keyboard_c		0x2e
#define keyboard_d		0x20
#define keyboard_e		0x12
#define keyboard_f		0x21
#define keyboard_g		0x22
#define keyboard_h		0x23
#define keyboard_i		0x17
#define keyboard_j		0x24
#define keyboard_k		0x25
#define keyboard_l		0x26
#define keyboard_m		0x32
#define keyboard_n		0x31
#define keyboard_o		0x18
#define keyboard_p		0x19
#define keyboard_q		0x10
#define keyboard_r		0x13
#define keyboard_s		0x1f
#define keyboard_t		0x14
#define keyboard_u		0x16
#define keyboard_v		0x2f
#define keyboard_w		0x11
#define keyboard_x		0x2d
#define keyboard_y		0x15
#define keyboard_z		0x2c

#define keyboard_0		0x0b
#define keyboard_1		0x02
#define keyboard_2		0x03
#define keyboard_3		0x04
#define keyboard_4		0x05
#define keyboard_5		0x06
#define keyboard_6		0x07
#define keyboard_7		0x08
#define keyboard_8		0x09
#define keyboard_9		0x0a

#define keyboard_semicolon		0x27
#define keyboard_squarebrack_left	0x1a
#define keyboard_squarebrack_right	0x1b
#define keyboard_comma			0x33
#define keyboard_dot			0x34
#define keyboard_slash			0x35
#define keyboard_backslash		0x2b
#define keyboard_upper_dot		0x29
#define keyboard_apostrophe		0x28
#define keyboard_minus			0x0c
#define keyboard_equal			0x0d

#define keyboard_f1		0x3b
#define keyboard_f2		0x3c
#define keyboard_f3		0x3d
#define keyboard_f4		0x3e
#define keyboard_f5		0x3f
#define keyboard_f6		0x40
#define keyboard_f7		0x41
#define keyboard_f8		0x42
#define keyboard_f9		0x43
#define keyboard_f10		0x44
#define keyboard_f11		0x57
#define keyboard_f12		0x58

#define keyboard_ctrl		0x1d
#define keyboard_lctrl		0x001d
#define keyboard_alt		0x38
#define keyboard_lalt		0x0038
#define keyboard_lshift		0x2a
#define keyboard_rshift		0x36

#define keyboard_extension0	0xe0
#define keyboard_extension1	0xe1

#define keyboard_uparrow	0xe048
#define keyboard_downarrow	0xe04b
#define keyboard_leftarrow	0xe050
#define keyboard_rightarrow	0xe04d

#define keyboard_insert		0xe052
#define keyboard_home		0xe047
#define keyboard_pageup		0xe049
#define keyboard_pagedown	0xe051
#define keyboard_end		0xe04f

#define keyboard_rctrl		0xe01d
#define keyboard_lsuper		0xe05b
#define keyboard_rsuper		0xe05c

typedef void (*keyboard_event_handler)(word scancode, dword is_down);
typedef void (*keyboard_input_handler)(byte character);

#ifdef __keyboard_c__
	#define __keyboard_h_export __public
#else
	#define __keyboard_h_export extern
#endif

__keyboard_h_export void keyboard_initalize(keyboard_event_handler event, keyboard_input_handler input);

#endif
