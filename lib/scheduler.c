#ifndef __scheduler_c__
#define __scheduler_c__

#include "process.h"

__public dt_pointer gdt_pointer;
__public descriptor gdt[process_maxcount];

tss global_tss;

process process_control_blocks[process_maxcount];
__private int current_process;
__private int total_process;

standard_ldt kernel_ldt;

extern void asm_scheduler_get_selectors();
extern void asm_scheduler_set_selectors();
extern void asm_scheduler_get_gdt();
extern void asm_scheduler_set_taskregs(selector ldt, selector ts);
extern void asm_scheduler_set_gdt();

extern selector scheduler_cs;
extern selector scheduler_ds;
extern selector scheduler_es;
extern selector scheduler_fs;
extern selector scheduler_ss;
extern selector scheduler_gs;

__private void scheduler_copy_descriptor(selector ldt_selector, selector selector);

__scheduler_export void scheduler_initialize()
{
	int i = 0; byte* gdt_bytes = gdt;
	for(; i < sizeof(gdt); i ++) gdt_bytes[i] = 0;

	asm_scheduler_get_selectors();
	asm_scheduler_get_gdt();

	/************		Reset Descriptor Tables Of Kernel    ******************/
	scheduler_copy_descriptor(stdldt_selector_cs, scheduler_cs);
	scheduler_copy_descriptor(stdldt_selector_ds, scheduler_ds);
	scheduler_es = scheduler_ds;
	scheduler_copy_descriptor(stdldt_selector_es, scheduler_es);
	scheduler_copy_descriptor(stdldt_selector_fs, scheduler_fs);
	scheduler_copy_descriptor(stdldt_selector_ss, scheduler_ss);
	scheduler_copy_descriptor(stdldt_selector_gs, scheduler_gs);

	gdt_pointer.base = &gdt;
	gdt_pointer.limit = 9 * sizeof(descriptor) - 1;		//1 Null LDT, 6 Kernel GDT, 1 Global TSS, 1 Empty LDT.
	
	descriptor_new(&gdt[0], 0, 0, 0, 0);
	// 1 ~ 6 For GDT.
	descriptor_new(&gdt[7], &global_tss, sizeof(tss) - 1, descriptor_tss | descriptor_present, privilege_system);
	descriptor_new(&gdt[8], &kernel_ldt, sizeof(kernel_ldt) - 1, descriptor_ldt | descriptor_present, privilege_system);

	/*************		Reset Segment Registers			*********************/
	scheduler_cs = selector_new(stdldt_selector_cs + 8, selector_global, privilege_system);
	scheduler_ds = selector_new(stdldt_selector_ds + 8, selector_global, privilege_system);
	scheduler_es = selector_new(stdldt_selector_es + 8, selector_global, privilege_system);
	scheduler_fs = selector_new(stdldt_selector_fs + 8, selector_global, privilege_system);
	scheduler_ss = selector_new(stdldt_selector_ss + 8, selector_global, privilege_system);
	scheduler_gs = selector_new(stdldt_selector_gs + 8, selector_global, privilege_user);

	asm_scheduler_set_gdt();
	selector ldt = selector_new(8 * sizeof(descriptor), selector_global, privilege_system);
	selector tr = selector_new(7 * sizeof(descriptor), selector_global, privilege_system);
	asm_scheduler_set_taskregs(ldt, tr);
	asm_scheduler_set_selectors();

	/**************		Reset Process Control Block Of Kernel	**********************/
	current_process = 8;
	process_control_blocks[8].state = process_state_running | process_state_daemon | process_entry_valid;
	total_process = 9;
}

void scheduler_copy_descriptor(selector ldt_selector, selector selector)
{
	byte* source_base = gdt_pointer.base + (selector & 0x0000fff8);
	byte* destin_base = gdt;
	destin_base += (ldt_selector & 0x0000fff8) + 8;
	int i = 0;
	for(; i < 8; i ++) destin_base[i] = source_base[i];
}

extern interrupt_stack_frame* asm_scheduler_get_stackframe();
extern void asm_scheduler_copy_stackframe(selector destin_selector, selector begin, selector end);

#include "driver/video.h"
/**		This Method Could Only Be Called In Kernel			***/
__scheduler_export void scheduler_execute(byte* pname, standard_ldt* stdldt, dword eip, dword esp, dword eflags,
	dword kernel_esp, word initstate)
{
	/**	We Can Initialize The Process By Pushing A Interrupt Stack Frame (Or Processor State
	Word Into The Stack Corresponding To The Stack Segment Of The Allocated Process, So We Could
	Schedule It Just Like A Running Process.	**/
	int idx = total_process;
	total_process ++;
	process_control_blocks[idx].kernel_ss = selector_new(stdldt_selector_ks, selector_local, privilege_system);
	process_control_blocks[idx].kernel_esp = kernel_esp;
	process_control_blocks[idx].ss = selector_new(stdldt_selector_ss, selector_local, privilege_system);
	process_control_blocks[idx].esp = esp - sizeof(interrupt_stack_frame) - 4;

	int i = 0;
	byte* destin = &(kernel_ldt.kernel_stack_segment);
	byte* source = &(stdldt->stack_segment);
	for(; i < 8; i ++) destin[i] = source[i];
	selector destin_selector = selector_new(stdldt_selector_ks, selector_local, descriptor_get_privilege(&stdldt->stack_segment));

	interrupt_stack_frame* scheduler_temp_stack_frame = asm_scheduler_get_stackframe();

	scheduler_temp_stack_frame->cs = selector_new(stdldt_selector_cs, selector_local, descriptor_get_privilege(&stdldt->code_segment));
	scheduler_temp_stack_frame->eip = eip;
	scheduler_temp_stack_frame->ds = selector_new(stdldt_selector_ds, selector_local, descriptor_get_privilege(&stdldt->data_segment));
	scheduler_temp_stack_frame->es = selector_new(stdldt_selector_es, selector_local, descriptor_get_privilege(&stdldt->edata_segment));
	scheduler_temp_stack_frame->fs = selector_new(stdldt_selector_fs, selector_local, descriptor_get_privilege(&stdldt->fdata_segment));
	scheduler_temp_stack_frame->gs = selector_new(stdldt_selector_gs, selector_local, descriptor_get_privilege(&stdldt->graphic_segment));
	process_control_blocks[idx].ldt
		= selector_new(idx * sizeof(descriptor), selector_global, descriptor_get_privilege(&stdldt->code_segment));
	scheduler_temp_stack_frame->ldt = process_control_blocks[idx].ldt;
	scheduler_temp_stack_frame->eflags = eflags;

	asm_scheduler_copy_stackframe(destin_selector, process_control_blocks[idx].esp, esp);

	descriptor_new(&gdt[idx], (dword)stdldt, sizeof(standard_ldt) - 1, descriptor_ldt | descriptor_present, privilege_system);
	process_control_blocks[idx].stack_frame = process_control_blocks[idx].esp;
	process_control_blocks[idx].state =
		process_state_ready | (initstate & process_state_switchable) | process_entry_valid;

	gdt_pointer.base = &gdt;
	gdt_pointer.limit = total_process * sizeof(descriptor) - 1;
	asm_scheduler_set_gdt();
}

__private void scheduler_pick()
{
	int i;
	int hasFound = 0;
	for(i = current_process + 1; i < total_process; i ++)
		if((process_control_blocks[i].state & process_state_fsm) == process_state_ready)
		{
			current_process = i;
			hasFound = 1;
			break;
		}
	if(hasFound == 0)
		for(i = 9; i < current_process; i ++)
			if((process_control_blocks[i].state & process_state_fsm) == process_state_ready)
		{
			current_process = i;
			hasFound = 1;
			break;
		}
	if(hasFound == 0) current_process = 8;
}

/*******	Calling This Method Will Force Current Process To Discard Processor!	***/
__scheduler_export void scheduler_terminate(selector* ldt, selector* ss,
	dword* esp, interrupt_stack_frame* stack_frame)
{
	/**	Recycle The Resource Of The Current Process.			**/
	if(current_process < total_process - 1)
	{
		byte* destin = &process_control_blocks[current_process];
		byte* source = &process_control_blocks[total_process - 1];

		int i = 0;
		for(i = 0; i < sizeof(process); i ++) destin[i] = source[i];
	}
	total_process --;

	scheduler_pick();

	/**	Pick Up Another Process	**/
	process_control_blocks[current_process].state
			= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_running;

	global_tss.stacks[0].esp = process_control_blocks[current_process].kernel_esp;
	global_tss.stacks[0].ss = process_control_blocks[current_process].kernel_ss;

	*ldt = 0;
	*ldt = process_control_blocks[current_process].ldt;
	*ss = process_control_blocks[current_process].ss;
	*esp = process_control_blocks[current_process].esp;
}

/*******	Calling This Method Will Force Current Process To Discard Processor!	***/
__scheduler_export void scheduler_sleep(dword mills, selector* ldt, selector* ss,
	dword* esp, interrupt_stack_frame* stack_frame)
{
	/**	Save Processor State	**/
	process_control_blocks[current_process].stack_frame = stack_frame;
	process_control_blocks[current_process].ldt = *ldt;	
	process_control_blocks[current_process].esp = *esp;
	process_control_blocks[current_process].ss = *ss;

	process_control_blocks[current_process].state
		= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_sleeping;
	process_control_blocks[current_process].tag = mills;

	scheduler_pick();

	/**	Pick Up Another Process	**/
	process_control_blocks[current_process].state
			= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_running;

	global_tss.stacks[0].esp = process_control_blocks[current_process].kernel_esp;
	global_tss.stacks[0].ss = process_control_blocks[current_process].kernel_ss;

	*ldt = 0;
	*ldt = process_control_blocks[current_process].ldt;
	*ss = process_control_blocks[current_process].ss;
	*esp = process_control_blocks[current_process].esp;
}

#include "driver/video.h"
/*******	Calling This Method Will Force Current Process To Discard Processor!	***/
__scheduler_export void scheduler_wait(word* waiting_queue_head, word* waiting_queue_tail, selector* ldt, selector* ss,
	dword* esp, interrupt_stack_frame* stack_frame)
{
	/**	Save Processor State	**/
	process_control_blocks[current_process].stack_frame = stack_frame;
	process_control_blocks[current_process].ldt = *ldt;	
	process_control_blocks[current_process].esp = *esp;
	process_control_blocks[current_process].ss = *ss;

	process_control_blocks[current_process].state
		= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_waiting;

	if(*waiting_queue_head == 0) *waiting_queue_head = current_process;
	if(*waiting_queue_tail != 0)
		process_control_blocks[*waiting_queue_tail].tag = current_process;
	*waiting_queue_tail = current_process;
	process_control_blocks[current_process].tag = 0;

	scheduler_pick();

	/**	Pick Up Another Process	**/
	process_control_blocks[current_process].state
			= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_running;

	global_tss.stacks[0].esp = process_control_blocks[current_process].kernel_esp;
	global_tss.stacks[0].ss = process_control_blocks[current_process].kernel_ss;

	*ldt = 0;
	*ldt = process_control_blocks[current_process].ldt;
	*ss = process_control_blocks[current_process].ss;
	*esp = process_control_blocks[current_process].esp;
}

/*******	Calling This Method Will Force Current Process To Discard Processor!	***/
__scheduler_export void scheduler_invoke(word* waiting_queue_head, word* waiting_queue_tail, selector* ldt, selector* ss,
	dword* esp, interrupt_stack_frame* stack_frame)
{
	/**	Save Processor State	**/
	process_control_blocks[current_process].stack_frame = stack_frame;
	process_control_blocks[current_process].ldt = *ldt;	
	process_control_blocks[current_process].esp = *esp;
	process_control_blocks[current_process].ss = *ss;

	process_control_blocks[current_process].state
		= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_ready;

	current_process = *waiting_queue_head;
	*waiting_queue_head = process_control_blocks[current_process].tag;
	if(*waiting_queue_head == 0) *waiting_queue_tail = 0;

	// Actually We Could Run Other Processes Instead.
	//process_control_blocks[current_process].state
	//		= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_ready;
	// scheduler_pick();

	/**	Pick Up Another Process	**/
	process_control_blocks[current_process].state
			= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_running;

	global_tss.stacks[0].esp = process_control_blocks[current_process].kernel_esp;
	global_tss.stacks[0].ss = process_control_blocks[current_process].kernel_ss;

	*ldt = 0;
	*ldt = process_control_blocks[current_process].ldt;
	*ss = process_control_blocks[current_process].ss;
	*esp = process_control_blocks[current_process].esp;
}

__scheduler_export interrupt_stack_frame* scheduler_schedule(selector* ldt, selector* ss, dword* esp,
	interrupt_stack_frame* stack_frame)
{
	/**	Save Processor State	**/
	process_control_blocks[current_process].stack_frame = stack_frame;
	process_control_blocks[current_process].ldt = *ldt;	
	process_control_blocks[current_process].esp = *esp;
	process_control_blocks[current_process].ss = *ss;

	process_control_blocks[current_process].state
		= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_ready;

	/**	Common Process For All Processes **/
	int i = 9;
	for(; i < total_process; i ++)
		if((process_control_blocks[i].state & process_state_fsm) == process_state_sleeping)
	{
		if(process_control_blocks[i].tag > 0)
			process_control_blocks[i].tag --;
		else	process_control_blocks[i].state
				= process_control_blocks[i].state & process_state_fsm_negate | process_state_ready;
	}

	/**	Determine The Next Process To Invoke	**/
	scheduler_pick();

	/**	Prepare Kernel Stack And Runtime Stack Frame	**/
	process_control_blocks[current_process].state
			= process_control_blocks[current_process].state & process_state_fsm_negate | process_state_running;

	global_tss.stacks[0].esp = process_control_blocks[current_process].kernel_esp;
	global_tss.stacks[0].ss = process_control_blocks[current_process].kernel_ss;

	*ldt = 0;
	*ldt = process_control_blocks[current_process].ldt;
	*ss = process_control_blocks[current_process].ss;
	*esp = process_control_blocks[current_process].esp;

	stack_frame = process_control_blocks[current_process].stack_frame;
	return stack_frame;
}

#endif
