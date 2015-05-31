/********************************************************************
 *			segmentation.h
 * @Title: segmentation c-language include file
 * @Author: LuoHaoran
 * @Description: This file declares structures and operation which
 * is related to segmentation in protected mode of cpu.
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

typedef word selector;

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

#pragma pack(pop)

__segmentation_h_func__ dword descriptor_get_limit(descriptor* d);

__segmentation_h_func__ void descriptor_set_limit(descriptor* d, dword limit);

__segmentation_h_func__ dword descriptor_get_base(descriptor *d);

__segmentation_h_func__ void descriptor_set_base(descriptor *d, dword base);

__segmentation_h_func__ word descriptor_get_attribute(descriptor *d);

__segmentation_h_func__ void descriptor_set_attribute(descriptor *d, word attribute);

__segmentation_h_func__ void descriptor_new(descriptor *d, dword base, dword limit, word attribute);

__segmentation_h_func__ selector selector_new(dword offset, word attribute);

#endif
