#ifndef __scheduler_c__
#define __scheduler_c__

#include "process.h"

__public dt_pointer gdt_pointer;
__public descriptor gdt[0x1000];

tss global_tss;

process kernel;
descriptor kernel_ldt[6];

__private int current_process;

extern void asm_scheduler_get_selectors();
extern void asm_scheduler_set_selectors();
extern void asm_scheduler_get_gdt();
extern void asm_scheduler_set_ldt(selector ldt);
extern void asm_scheduler_set_gdt();

extern selector scheduler_cs;
extern selector scheduler_ds;
extern selector scheduler_es;
extern selector scheduler_fs;
extern selector scheduler_ss;
extern selector scheduler_gs;

__private void scheduler_copy_descriptor(int ldt_index, selector selector);

__scheduler_export void scheduler_initialize()
{
	asm_scheduler_get_selectors();
	asm_scheduler_get_gdt();

	scheduler_copy_descriptor(0, scheduler_cs);
	scheduler_copy_descriptor(1, scheduler_ds);
	scheduler_es = scheduler_ds;
	scheduler_copy_descriptor(2, scheduler_es);
	scheduler_copy_descriptor(3, scheduler_fs);
	scheduler_copy_descriptor(4, scheduler_ss);
	scheduler_copy_descriptor(5, scheduler_gs);

	gdt_pointer.base = &gdt;
	gdt_pointer.limit = 3 * sizeof(descriptor) - 1;		//1 Null LDT, 1 Global TSS, 1 LDT
	
	descriptor_new(&gdt[0], 0, 0, 0, 0);
	descriptor_new(&gdt[1], &global_tss, sizeof(tss) - 1, descriptor_tss | descriptor_present, privilege_system);
	descriptor_new(&gdt[2], kernel_ldt, sizeof(kernel_ldt) - 1, descriptor_ldt | descriptor_present, privilege_system);

	scheduler_cs = selector_new(0 * sizeof(descriptor), selector_local, privilege_system);
	scheduler_ds = selector_new(1 * sizeof(descriptor), selector_local, privilege_system);
	scheduler_es = selector_new(2 * sizeof(descriptor), selector_local, privilege_system);
	scheduler_fs = selector_new(3 * sizeof(descriptor), selector_local, privilege_system);
	scheduler_ss = selector_new(4 * sizeof(descriptor), selector_local, privilege_system);
	scheduler_gs = selector_new(5 * sizeof(descriptor), selector_local, privilege_user);

	asm_scheduler_set_gdt();
	selector ldt = selector_new(0x10, selector_global, privilege_system);
	asm_scheduler_set_ldt(ldt);
	asm_scheduler_set_selectors();
}

void scheduler_copy_descriptor(int ldt_index, selector selector)
{
	byte* source_base = gdt_pointer.base + (selector & 0x0000fff8);
	byte* destin_base = &kernel_ldt[ldt_index];
	int i = 0;
	for(; i < 8; i ++) destin_base[i] = source_base[i];
}

__scheduler_export void scheduler_execute(char* pname, selector ldt, dword eip)
{
	
}

__scheduler_export interrupt_stack_frame* scheduler_schedule(interrupt_stack_frame* stack_frame)
{
	
}

#endif
