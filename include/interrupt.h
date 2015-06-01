#ifndef __interrupt_h__
#define __interrupt_h__

#include "define.h"
#include "segmentation.h"

#ifdef __interrupt_c__
	#define __interrupt_h_export __public
#else
	#define __interrupt_h_export extern
#endif

#pragma pack(push)
#pragma pack(1)

__constant(interrupt_master_8295a, 0x20);
__constant(interrupt_slave_8295a, 0xa0);

typedef byte icw_general;

__constant(icw_general_base,			0x10);

__constant(icw_general_icw_mode_present,	1);
__constant(icw_general_icw_mode_none,		0);

__constant(icw_general_pic_single,		1);
__constant(icw_general_pic_cascade,		0);

__constant(icw_general_call_int4,		1);
__constant(icw_general_call_int8,		0);

__constant(icw_general_trigger_level,		1);
__constant(icw_general_trigger_edge,		0);

__constant(icw_vector_system_8086,		0);
typedef byte icw_vector;

__constant(icw_master_mask_ir0,			0x01);
__constant(icw_master_mask_ir1,			0x02);
__constant(icw_master_mask_ir2,			0x04);
__constant(icw_master_mask_ir3,			0x08);
__constant(icw_master_mask_ir4,			0x10);
__constant(icw_master_mask_ir5,			0x20);
__constant(icw_master_mask_ir6,			0x40);
__constant(icw_master_mask_ir7,			0x80);
typedef byte icw_master_mask;

typedef byte icw_slave_mask;

typedef byte icw_mode;

__constant(icw_mode_mcs8085,		0);
__constant(icw_mode_8086,		1);

__constant(icw_mode_eoi_normal,		0);
__constant(icw_mode_eoi_auto,		2);

__constant(icw_mode_buffer_none,	0);
__constant(icw_mode_buffer_slave,	0x08);
__constant(icw_mode_buffer_master,	0x0c);

__constant(icw_mode_fully_nested_special,	0x10);
__constant(icw_mode_fully_nested_normal,	0x00);

__interrupt_h_export void interrupt_controller_initialize();
__interrupt_h_export void interrupt_controller_set(byte mask, byte enabled);
__interrupt_h_export void interrupt_controller_end(byte mask);

typedef void (*exception_handler)(dword vector, dword error_code, dword ip, selector cs, dword eflag);
typedef void (*interrupt_handler)(dword interrupt);
__interrupt_h_export void interrupt_idt_set_pointer(dt_pointer* pointer, selector cs);

__constant(interrupt_ir0_clock,		0x00);
__constant(interrupt_ir1_keyboard,	0x01);
__constant(interrupt_ir2_slave,		0x02);
__constant(interrupt_ir3_serial2,	0x03);
__constant(interrupt_ir4_serial1,	0x04);
__constant(interrupt_ir5_lpt2,		0x05);
__constant(interrupt_ir6_floppy,	0x06);
__constant(interrupt_ir7_lpt2,		0x07);

__constant(interrupt_ir8_real_clock,	0x08);
__constant(interrupt_ir9_redirect,	0x09);
__constant(interrupt_ira_reserved,	0x0a);
__constant(interrupt_irb_reserved,	0x0b);
__constant(interrupt_irc_cursor,	0x0c);
__constant(interrupt_ird_fpu,		0x0d);
__constant(interrupt_ire_windchester,	0x0e);
__constant(interrupt_irf_reserved,	0x0f);

#pragma pack(pop)

#endif
