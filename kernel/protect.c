#include "define.h"
#include "segmentation.h"

dt_pointer gdt_pointer;
descriptor gdt[0x1000];

__public dt_pointer* kernel_gdt_migration(dt_pointer* old_pointer)
{
	int i;
	gdt_pointer.base = gdt;
	for(i = 0; i <= old_pointer->limit; i ++)
		((byte*)(gdt_pointer.base))[i] = ((byte*)(old_pointer->base))[i];
	gdt_pointer.limit = old_pointer->limit;
	return &gdt_pointer;
}
