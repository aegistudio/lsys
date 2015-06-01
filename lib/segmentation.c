/********************************************************************
 *			segmentation.c
 * @Title: segmentation c-language source file
 * @Author: LuoHaoran
 * @Description: This file implements the operations on descriptors,
 gates, and so on, which is related to segmentation.
********************************************************************/

#ifndef __segmentation_c__
#define __segmentation_c__

#include "segmentation.h"

dword descriptor_get_limit(descriptor* d)
{
	dword return_value = d->limit_low;
	return_value |= ((d->attribute_limit_high & 0x0f00) >> 8) << 16;
	return return_value;
}

void descriptor_set_limit(descriptor* d, dword limit)
{
	d->limit_low = limit & 0xffff;
	d->attribute_limit_high &= 0xf0ff;
	d->attribute_limit_high |= ((limit >> 16) << 8) & 0x0f00;
}

dword descriptor_get_base(descriptor* d)
{
	dword return_value = d->base_low;
	return_value |= (d->base_mid) << 16;
	return_value |= (d->base_high) << 24;
	return return_value;
}

void descriptor_set_base(descriptor *d, dword base)
{
	d->base_low = base & 0x0000ffff;
	d->base_mid = (base & 0x00ff0000) >> 16;
	d->base_high = (base & 0xff000000) >> 24;
}

dword descriptor_get_attribute(descriptor *d)
{
	return (d->attribute_limit_high) & 0xf0ff;
}

void descriptor_set_attribute(descriptor *d, dword attribute, dword privilege)
{
	attribute |= (privilege & 0x03) << 5;
	d->attribute_limit_high &= 0x0f00;
	d->attribute_limit_high |= (attribute & 0xf0ff);
}

void descriptor_new(descriptor *d, dword base, dword limit, dword attribute, dword privilege)
{
	descriptor_set_base(d, base);
	descriptor_set_limit(d, limit);
	descriptor_set_attribute(d, attribute, privilege);
}

selector selector_new(dword offset, dword is_ldt, dword privilege)
{
	return (offset & 0xfff8) | is_ldt | privilege;
}

dword gate_get_offset(gate* g)
{
	dword offset = g->offset_low;
	offset |= g->offset_high << 16;
	return  offset;
}

void gate_set_offset(gate* g, dword offset)
{
	g->offset_low = offset & 0x0000ffff;
	g->offset_high = (offset & 0xffff0000) >> 16;
}

selector gate_get_base(gate* g)
{
	return g->base;
}

void gate_set_base(gate* g, selector base)
{
	g->base = base;
}

byte gate_get_parameter_count(gate* g)
{
	return g->parameter_count;
}

void gate_set_parameter_count(gate* g, byte count)
{
	g->parameter_count = count & 0x1f;
}

dword gate_get_attribute(gate* g)
{
	return g->attribute;
}

void gate_set_attribute(gate* g, dword attribute, dword privilege)
{
	g->attribute = attribute & 0x9f | ((privilege & 0x03) << 5);
}

void gate_new(gate* g, dword offset, selector base, dword attribute, dword privilege)
{
	gate_set_offset(g, offset);
	gate_set_base(g, base);
	gate_set_attribute(g, attribute, privilege);
}

#endif
