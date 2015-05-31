#include "define.h"
#include "segmentation.h"

__public void kernel_gdt_migration(dt_pointer* old_pointer, dt_pointer* new_pointer)
{
	int i;
	for(i = 0; i <= old_pointer->limit; i ++)
		((byte*)(new_pointer->base))[i] = ((byte*)(old_pointer->base))[i];
	new_pointer->limit = old_pointer->limit;
}
