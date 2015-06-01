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

typedef struct icw_vector_t
{
	byte system : 3;
	byte vector : 5;
}
icw_vector;

typedef struct icw_master_mask_t
{
	byte ir0 : 1;
	byte ir1 : 1;
	byte ir2 : 1;
	byte ir3 : 1;
	byte ir4 : 1;
	byte ir5 : 1;
	byte ir6 : 1;
	byte ir7 : 1;
}
icw_master_mask;

typedef struct icw_slave_mask
{
	byte master_ir : 3;
	byte zeroing : 5;
}
icw_slave_mask;

typedef struct icw_mode_t
{
	byte mode : 1;
	byte eoi : 1;
	byte buffer : 2;
	byte fully_nested : 1;
	byte zeroing : 3;
}
icw_mode;

__constant(icw_mode_mcs8085,		0);
__constant(icw_mode_8086,		1);

__constant(icw_mode_eoi_normal,		0);
__constant(icw_mode_eoi_auto,		1);

__constant(icw_mode_buffer_none,	0);
__constant(icw_mode_buffer_slave,	2);
__constant(icw_mode_buffer_master,	3);

__constant(icw_mode_fully_nested_special,	0);
__constant(icw_mode_fully_nested_normal,	0);

__interrupt_h_export void interrupt_controller_initialize();

#pragma pack(pop)

#endif
