#ifndef __interrupt_c__
#define __interrupt_c__

#include "interrupt.h"

extern void asm_interrupt_outb(word port, byte word);
extern byte asm_interrupt_inb(word port);

__constant(interrupt_master_8295a, 0x20);
__constant(interrupt_slave_8295a, 0xa0);

void interrupt_controller_initialize()
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
	asm_interrupt_outb(interrupt_master_8295a, mode);
	asm_interrupt_outb(interrupt_slave_8295a, mode);
}

#endif
