#ifndef __interrupt_h__
#define __interrupt_h__

#include "define.h"

#ifdef __interrupt_c__
	#define __interrupt_h_export __public
#else
	#define __interrupt_h_export extern
#endif

#pragma pack(push)
#pragma pack(1)

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

#pragma pack(pop)

#endif
