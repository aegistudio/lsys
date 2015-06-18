/********************************************************************
 *			process.h
 * @Title: process c-language include file
 * @Author: LuoHaoran
 * @Description: This file declares structures and operation which
 * is related to segmentation in protected mode of cpu.
********************************************************************/

#ifndef __process_h__
#define __process_h__

#include "define.h"
#include "segmentation.h"

#pragma pack(push)
#pragma pack(1)

typedef struct __segment_state_table_t
{
	selector es;
	word preserved_es;

	selector cs;
	word preserved_cs;

	selector ss;
	word preserved_ss;

	selector ds;
	word preserved_ds;

	selector fs;
	word preserved_fs;

	selector gs;
	word preserved_gs;
}
segment_state_table;

typedef struct __stack_state_table_t
{
	dword esp;
	selector ss;
	word preserved_ss;
}
stack_state_table;

typedef struct __register_state_table_t
{
	dword eax;
	dword ecx;
	dword edx;
	dword ebx;
	dword esp;
	dword ebp;
	dword esi;
	dword edi;
}

register_state_table;

/********************************************************************
 *			tss
 * @Name: task-state segment
 * @Author: LuoHaoran
 * @Description: Task-state segment (TSS) is used for register state
 * saving and privilege trasition.
********************************************************************/

typedef struct __tss_t
{
	word back_link;
	word preserved_back_link;

	stack_state_table stacks[3];

	dword pdbr;
	dword eip;
	dword eflags;

	register_state_table registers;

	segment_state_table segments;
	selector ldt;
	word preserved_ldt;

	word trace;
	word iopl_base;
}
tss;

/************************************************************************
 *			process
 * @Name: process control block
 * @Author: LuoHaoran
 * @Description: Process control block (PCB) is used for process runtime
 * information maintaince. This struct can be extended to maintain more
 * information. Like priority, and so on, but the basic structure remains
 * the same.
************************************************************************/

#include "driver/interrupt.h"

#define process_state_switchable		0x0001
#define process_state_daemon			0x0000
#define process_state_userinterface		0x0001

#define process_state_fsm			0x000e
#define process_state_ready			0x0000
#define process_state_running			0x0002
#define process_state_blocked			0x0004

#define stdldt_selector_cs			0x0000
#define stdldt_selector_ds			0x0008
#define stdldt_selector_es			0x0010
#define stdldt_selector_fs			0x0018
#define stdldt_selector_gs			0x0020
#define stdldt_selector_ss			0x0028
#define stdldt_selector_ks			0x0030
#define stdldt_selector_is			0x0038

typedef struct __standard_ldt_t
{
	descriptor code_segment;
	descriptor data_segment;
	descriptor edata_segment;
	descriptor fdata_segment;
	descriptor graphic_segment;
	descriptor stack_segment;
	descriptor kernel_stack_segment;
	descriptor interrupt_segment;
}
standard_ldt;

typedef struct __pcb_t
{
	byte name[12];
	byte state;
	byte invoke;
	interrupt_stack_frame* stack_frame;

	/** Will Be Updated When Process Switches! **/
	selector kernel_ss;
	dword kernel_esp;
}
process;

#ifdef __scheduler_c__
	#define __scheduler_export __public
#else
	#define __scheduler_export extern
#endif

//@Warning: Who call this method will be first process, whose name will be 'system'.
__scheduler_export void scheduler_initialize();

//@Warning: This method will only push a process into waiting queue or ready queue, but will not execute instantly.
__scheduler_export void scheduler_execute(char* pname, selector ldt, dword eip);

//@Warning: This method will invoke scheduler to pickup a process and update current running process state.
__scheduler_export interrupt_stack_frame* scheduler_schedule(interrupt_stack_frame* stack_frame);

#pragma pack(pop)

#endif

