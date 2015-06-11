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

#pragma pack(pop)

#endif

