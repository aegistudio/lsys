/********************************************************************
 *			segmentation.h
 * @Title: segmentation c-language include file
 * @Author: LuoHaoran
 * @Description: This file declares structures and operation which
 * is related to segmentation in protected mode of cpu.
********************************************************************/

#ifndef __segmentation_h__
#define __segmentation_h__

#include "define.h"

#ifndef __segmentation_c__
	#define __segmentation_h_func__ extern
#else
	#define __segmentation_h_func__ __public
#endif

#pragma pack(push)
#pragma pack(1)

#define privilege_system				0x00
#define privilege_kernel				0x00
#define privilege_root_service				0x01
#define privilege_service				0x02
#define privilege_user					0x03
#define privilege_application				0x03

typedef word selector;

__segmentation_h_func__ selector selector_new(dword offset, dword is_ldt, dword privilege);

#define selector_local					0x04
#define selector_global					0x00

typedef struct __dt_pointer_t
{
	word limit;
	dword base;
}
dt_pointer;

typedef struct __descriptor_t
{
	word limit_low;
	word base_low;
	byte base_mid;
	word attribute_limit_high;
	byte base_high;
}
descriptor;

__segmentation_h_func__ dword descriptor_get_limit(descriptor* d);
__segmentation_h_func__ void descriptor_set_limit(descriptor* d, dword limit);
__segmentation_h_func__ dword descriptor_get_base(descriptor *d);
__segmentation_h_func__ void descriptor_set_base(descriptor *d, dword base);
__segmentation_h_func__ dword descriptor_get_attribute(descriptor *d);
__segmentation_h_func__ dword descriptor_get_privilege(descriptor *d);
__segmentation_h_func__ void descriptor_set_attribute(descriptor *d, dword attribute, dword privilege);
__segmentation_h_func__ void descriptor_new(descriptor *d, dword base, dword limit, dword attribute, dword privilege);

#define descriptor_data32				0x4010
#define descriptor_data16				0x0010
#define descriptor_expdown4gb				0x4014
#define descriptor_expdown64k				0x0014
#define descriptor_code32				0x4018
#define descriptor_code16				0x0018
#define descriptor_system				0x0000

#define descriptor_accessed				0x01

#define descriptor_data_readonly			0x00
#define descriptor_data_readwrite			0x02

#define descriptor_code_noread				0x00
#define descriptor_code_readable			0x02
#define descriptor_code_conform				0x04

#define descriptor_missing				0x00
#define descriptor_present				0x80

#define descriptor_gran_byte				0x0000
#define descriptor_gran_4kb				0x8000

#define descriptor_none					0x0000
#define descriptor_available				0x1000

typedef struct __gate_t
{
	word offset_low;
	selector base;
	byte parameter_count;
	byte attribute;
	word offset_high;
}
gate;

__segmentation_h_func__ dword gate_get_offset(gate* g);
__segmentation_h_func__ void gate_set_offset(gate* g, dword offset);
__segmentation_h_func__ selector gate_get_base(gate* g);
__segmentation_h_func__ void gate_set_base(gate* g, selector base);
__segmentation_h_func__ byte gate_get_parameter_count(gate* g);
__segmentation_h_func__ void gate_set_parameter_count(gate* g, byte count);
__segmentation_h_func__ dword gate_get_attribute(gate* g);
__segmentation_h_func__ void gate_set_attribute(gate* g, dword attribute, dword privilege);
__segmentation_h_func__ void gate_new(gate* g, dword offset, selector base, dword attribute, dword privilege);

#define descriptor_system_286				0x00
#define descriptor_system_386				0x08

#define descriptor_tss					0x01
#define descriptor_system_busy				0x02
#define descriptor_system_idle				0x00

#define descriptor_ldt					0x02
#define descriptor_gate_call				0x04
#define descriptor_gate_interrupt			0x06
#define descriptor_gate_trap				0x07

#pragma pack(pop)

#endif
