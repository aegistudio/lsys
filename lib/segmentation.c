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

#endif
