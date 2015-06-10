
#ifndef __interrupt_c__
#define __interrupt_c__

#include "driver/interrupt.h"

extern void asm_interrupt_outb(word port, byte word);
extern byte asm_interrupt_inb(word port);

__public void interrupt_controller_initialize()
{
	byte* byte_pointer;
	icw_general general = icw_general_base
		| icw_general_icw_mode_present | icw_general_pic_cascade
		| icw_general_call_int8 | icw_general_trigger_edge;
	asm_interrupt_outb(interrupt_master_8295a, general);
	asm_interrupt_outb(interrupt_slave_8295a, general);

	icw_vector master_vector = icw_vector_system_8086 | 0x20;
	icw_vector slave_vector = icw_vector_system_8086 | 0x28;
	asm_interrupt_outb(interrupt_master_8295a + 1, master_vector);
	asm_interrupt_outb(interrupt_slave_8295a + 1, slave_vector);

	icw_master_mask master_mask = icw_master_mask_ir2;
	asm_interrupt_outb(interrupt_master_8295a + 1, master_mask);

	icw_slave_mask slave_mask = 2;
	asm_interrupt_outb(interrupt_slave_8295a + 1, slave_mask);

	icw_mode mode = icw_mode_8086
		| icw_mode_eoi_normal | icw_mode_buffer_none
		| icw_mode_fully_nested_normal;
	asm_interrupt_outb(interrupt_master_8295a + 1, mode);
	asm_interrupt_outb(interrupt_slave_8295a + 1, mode);

	asm_interrupt_outb(interrupt_master_8295a + 1, 0xff);
	asm_interrupt_outb(interrupt_slave_8295a + 1, 0xff);
}

__public void interrupt_controller_set(byte mask, byte enabled)
{
	if(mask < 0x08)
	{
		byte maskword = 1 << mask;
		byte interrupt_master_8295a_ocw = asm_interrupt_inb(interrupt_master_8295a + 1);
		if(enabled) interrupt_master_8295a_ocw &= (0xff ^ maskword);
		else interrupt_master_8295a_ocw |= maskword;
		asm_interrupt_outb(interrupt_master_8295a + 1, interrupt_master_8295a_ocw);
	}
	else
	{
		byte maskword = 1 << (mask - 0x08);
		byte interrupt_slave_8295a_ocw = asm_interrupt_inb(interrupt_slave_8295a + 1);
		if(enabled) interrupt_slave_8295a_ocw &= (0xff ^ maskword);
		else interrupt_slave_8295a_ocw |= maskword;
		asm_interrupt_outb(interrupt_slave_8295a + 1, interrupt_slave_8295a_ocw);
	}
}

__public void interrupt_controller_end(byte mask)
{
	if(mask < 0x08)
		asm_interrupt_outb(interrupt_master_8295a, 0x20);
	else asm_interrupt_outb(interrupt_slave_8295a, 0x20);
}

__private gate* idt;
__private exception_handler exception_handlers[20];

extern void exception_tag_0_division_error();
extern void exception_tag_1_debug();
extern void exception_tag_2_nmi();
extern void exception_tag_3_breakpoint();
extern void exception_tag_4_overflow();
extern void exception_tag_5_bounds_check();
extern void exception_tag_6_invalid_opcode();
extern void exception_tag_7_coprocessor_unavailable();
extern void exception_tag_8_double_fault();
extern void exception_tag_9_coprocessor_exceed();
extern void exception_tag_10_invalid_tss();
extern void exception_tag_11_not_present();
extern void exception_tag_12_stack_fault();
extern void exception_tag_13_general_protection();
extern void exception_tag_14_page_fault();
extern void exception_tag_15_reserved();
extern void exception_tag_16_fpu_fault();
extern void exception_tag_17_alignment_check();
extern void exception_tag_18_machine_check();
extern void exception_tag_19_simd_exception();

__public void exception_handler_bus(dword vector, dword error_code, dword ip, selector cs, dword eflag)
{
	exception_handler handler = exception_handlers[vector];
	if(handler != 0) handler(vector, error_code, ip, cs, eflag);
}

#include "driver/video.h"
const byte* exception_error_messages_0 = "#0: division_error";
const byte* exception_error_messages_1 = "#1: debug";
const byte* exception_error_messages_2 = "#2: nmi";
const byte* exception_error_messages_3 = "#3: breakpoint";
const byte* exception_error_messages_4 = "#4: overflow";
const byte* exception_error_messages_5 = "#5: bounds_check";
const byte* exception_error_messages_6 = "#6: invalid_opcode";
const byte* exception_error_messages_7 = "#7: coprocessor_unavailable";
const byte* exception_error_messages_8 = "#8: double_fault";
const byte* exception_error_messages_9 = "#9: coprocessor_exceed";
const byte* exception_error_messages_10 = "#10: invalid_tss";
const byte* exception_error_messages_11 = "#11: not_present";
const byte* exception_error_messages_12 = "#12: stack_fault";
const byte* exception_error_messages_13 = "#13: general_protection";
const byte* exception_error_messages_14 = "#14: page_fault";
const byte* exception_error_messages_15 = "#15: reserved";
const byte* exception_error_messages_16 = "#16: fpu_fault";
const byte* exception_error_messages_17 = "#17: alignment_check";
const byte* exception_error_messages_18 = "#18: machine_check";
const byte* exception_error_messages_19 = "#19: simd_exception";

char print_hex_string[16];

__private byte* convert_hex(dword hex, int bits)
{
	byte* pointer = print_hex_string + sizeof(print_hex_string) - 1;
	*pointer = 0;

	int i = 0;
	for(; i < bits; i ++)
	{
		pointer --;
		*pointer = (hex & 0x0000000f);
		if(*pointer < 10) *pointer += '0';
		else *pointer += 'a';
		hex = hex >> 4;
	}
	return pointer;
}

__private void default_exception_handler(dword vector, dword error_code, dword ip, selector cs, dword eflag)
{
	if(vector < 20)
	{
		video_put_string("\n======default_exception_handler=========", 0x07);

		video_put_string("\nexception_id: ", 0x07);
		video_put_char(vector / 10 + '0', 0x07);
		video_put_char(vector % 10 + '0', 0x07);
		video_put_string(" (hex: 0x", 0x07);
		video_put_string(convert_hex(vector, 2), 0x07);
		video_put_string(")", 0x07);

		video_put_string("\nip: 0x", 0x07);
		video_put_string(convert_hex(ip, 8), 0x07);

		video_put_string("\ncs: 0x", 0x07);
		video_put_string(convert_hex(cs & 0xfff8, 4), 0x07);
		video_put_string(" (privilege: ", 0x07);
		video_put_string(convert_hex(cs & 0x0003, 1), 0x07);
		video_put_string(")", 0x07);

		video_put_string("\neflag: ", 0x07);
		video_put_string(convert_hex(eflag, 8), 0x07);

		if(error_code != 0xffffffff)
		{
			video_put_string("\nerror_code: ", 0x07);
			video_put_string(convert_hex(error_code, 8), 0x07);
		}
		else video_put_string("\nno error code.", 0x07);

		video_put_string("\n========================================", 0x07);
	}
}

extern void asm_default_interrupt_handler();
__public void default_interrupt_handler()
{
	video_put_string("\n=======default_interrupt_handler==========", 0x07);
	video_put_string("\nThe corresponding handler was not set, no ", 0x07);
	video_put_string("\nprocess will be made and I/O data is likely", 0x07);
	video_put_string("\nto lose.", 0x07);
	video_put_string("\n==========================================", 0x07);
	interrupt_controller_end(0x00);
	interrupt_controller_end(0x08);
}

__private selector interrupt_code_selector;

#define __interrupt_gate_setup(index, tag) gate_new(&idt[index], tag, interrupt_code_selector, \
		descriptor_gate_interrupt | descriptor_system_386 | descriptor_present, \
		privilege_kernel);
__public void interrupt_idt_set_pointer(dt_pointer* pointer, selector cs)
{
	idt = (gate*)(pointer->base);
	interrupt_code_selector = cs;
	int i = 0;
	for(; i < 20; i ++)
		exception_handlers[i] = (exception_handler*)default_exception_handler;

	 __interrupt_gate_setup(0, exception_tag_0_division_error);
	 __interrupt_gate_setup(1, exception_tag_1_debug);
	 __interrupt_gate_setup(2, exception_tag_2_nmi);
	 __interrupt_gate_setup(3, exception_tag_3_breakpoint);
	 __interrupt_gate_setup(4, exception_tag_4_overflow);
	 __interrupt_gate_setup(5, exception_tag_5_bounds_check);
	 __interrupt_gate_setup(6, exception_tag_6_invalid_opcode);
	 __interrupt_gate_setup(7, exception_tag_7_coprocessor_unavailable);
	 __interrupt_gate_setup(8, exception_tag_8_double_fault);
	 __interrupt_gate_setup(9, exception_tag_9_coprocessor_exceed);
	 __interrupt_gate_setup(10, exception_tag_10_invalid_tss);
	 __interrupt_gate_setup(11, exception_tag_11_not_present);
	 __interrupt_gate_setup(12, exception_tag_12_stack_fault);
	 __interrupt_gate_setup(13, exception_tag_13_general_protection);
	 __interrupt_gate_setup(14, exception_tag_14_page_fault);
	 __interrupt_gate_setup(15, exception_tag_15_reserved);
	 __interrupt_gate_setup(16, exception_tag_16_fpu_fault);
	 __interrupt_gate_setup(17, exception_tag_17_alignment_check);
	 __interrupt_gate_setup(18, exception_tag_18_machine_check);
	 __interrupt_gate_setup(19, exception_tag_19_simd_exception);

	for(i = 0x20; i <= 0x2f; i ++) __interrupt_gate_setup(i, asm_default_interrupt_handler);
}

__public void interrupt_set_exception_handler(dword vector, exception_handler handler)
{
	if(vector < 20 && vector >= 0)
		exception_handlers[vector] = handler;
}

__public void interrupt_set_interrupt_handler(dword vector, interrupt_handler handler)
{
	if(vector < 0x20) return;
	int interrupt_gate_type;
	if(vector <= 0x2f) interrupt_gate_type = descriptor_gate_interrupt;
	else interrupt_gate_type = descriptor_gate_trap;
	gate_new(&idt[vector], handler, interrupt_code_selector, \
		descriptor_gate_trap | descriptor_system_386 | descriptor_present, \
		privilege_kernel);
}

#undef __interrupt_gate_setup

#endif
