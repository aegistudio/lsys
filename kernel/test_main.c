#include "process.h"

void test_main_init(standard_ldt* stdldt, dword code_base, dword code_limit, dword data_base, dword data_limit,
	dword stack_base, dword stack_limit, dword kernel_stack_base, dword kernel_stack_limit, dword graphic_base,
	dword graphic_limit, dword eflags)
{
	descriptor_new(&(stdldt->code_segment), code_base, code_limit,
		descriptor_code32 | descriptor_present | descriptor_gran_byte, privilege_system);
	descriptor_new(&(stdldt->data_segment), data_base, data_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_system);
	descriptor_new(&(stdldt->edata_segment), data_base, data_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_system);
	descriptor_new(&(stdldt->fdata_segment), data_base, data_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_system);
	descriptor_new(&(stdldt->stack_segment), stack_base, stack_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_system);
	descriptor_new(&(stdldt->kernel_stack_segment), kernel_stack_base, kernel_stack_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_system);
	descriptor_new(&(stdldt->graphic_segment), graphic_base, graphic_limit,
		descriptor_data32 | descriptor_present | descriptor_gran_byte | descriptor_data_readwrite, privilege_user);
	scheduler_execute("test_main", stdldt, 0, stack_limit - 1, eflags, kernel_stack_limit - 1, process_state_daemon);
}
