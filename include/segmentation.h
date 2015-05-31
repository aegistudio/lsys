/********************************************************************
 *			segmentation_h
 * @Title: segmentation c-language include file
 * @Author: LuoHa_ran
 * @Description: This file declares structures and operation which
 * is related to segmentation in protected mode of cpu_
********************************************************************/

#include "define.h"

#ifndef __segmentation_h__
#define __segmentation_h__

#ifndef __segmentation_c__
	#define __segmentation_h_func__ extern
#else
	#define __segmentation_h_func__ __public
#endif

#pragma pack(push)
#pragma pack(1)

__constant(privilege_system			,	0x00);
__constant(privilege_kernel			,	0x00);
__constant(privilege_root_service		,	0x01);
__constant(privilege_service			,	0x02);
__constant(privilege_user			,	0x03);
__constant(privilege_application		,	0x03);

typedef word selector;

__segmentation_h_func__ selector selector_new(dword offset, dword is_ldt, dword privilege);

__constant(selector_local			,	0x04);
__constant(selector_global			,	0x00);

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
__segmentation_h_func__ void descriptor_set_attribute(descriptor *d, dword attribute, dword privilege);
__segmentation_h_func__ void descriptor_new(descriptor *d, dword base, dword limit, dword attribute, dword privilege);

__constant(descriptor_data32			,	0x4010);
__constant(descriptor_data16			,	0x0010);
__constant(descriptor_expdown4gb		,	0x4014);
__constant(descriptor_expdown64k		,	0x0014);
__constant(descriptor_code32			,	0x4018);
__constant(descriptor_code16			,	0x0018);
__constant(descriptor_system			,	0x0000);

__constant(descriptor_accessed			,	0x01);

__constant(descriptor_data_readonly		,	0x00);
__constant(descriptor_data_readwrite		,	0x02);

__constant(descriptor_code_noread		,	0x00);
__constant(descriptor_code_readable		,	0x02);
__constant(descriptor_code_conform		,	0x04);

__constant(descriptor_missing			,	0x00);
__constant(descriptor_present			,	0x80);

__constant(descriptor_gran_byte			,	0x0000);
__constant(descriptor_gran_4kb			,	0x8000);

__constant(descriptor_none			,	0x0000);
__constant(descriptor_available			,	0x1000);

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

__constant(descriptor_system_286		,	0x00);
__constant(descriptor_system_386		,	0x08);

__constant(descriptor_tss			,	0x01);
__constant(descriptor_system_busy		,	0x02);
__constant(descriptor_system_idle		,	0x00);

__constant(descriptor_ldt			,	0x02);
__constant(descriptor_gate_call			,	0x04);
__constant(descriptor_gate_interrupt		,	0x06);
__constant(descriptor_gate_trap			,	0x07);

#pragma pack(pop)

#endif
