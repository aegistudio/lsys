#ifndef __interrupt_h__
#define __interrupt_h__

#include "define.h"
#include "segmentation.h"

#ifdef __interrupt_c__
	#define __interrupt_h_export __public
#else
	#define __interrupt_h_export extern
#endif

#define interrupt_master_8295a		 0x20
#define interrupt_slave_8295a		 0xa0

typedef byte icw_general;

#define icw_general_base		0x10

#define icw_general_icw_mode_present	1
#define icw_general_icw_mode_none	0

#define icw_general_pic_single		2
#define icw_general_pic_cascade		0

#define icw_general_call_int4		4
#define icw_general_call_int8		0

#define icw_general_trigger_level	8
#define icw_general_trigger_edge	0

#define icw_vector_system_8086		0
typedef byte icw_vector;

#define icw_master_mask_ir0		0x01
#define icw_master_mask_ir1		0x02
#define icw_master_mask_ir2		0x04
#define icw_master_mask_ir3		0x08
#define icw_master_mask_ir4		0x10
#define icw_master_mask_ir5		0x20
#define icw_master_mask_ir6		0x40
#define icw_master_mask_ir7		0x80
typedef byte icw_master_mask;

typedef byte icw_slave_mask;

typedef byte icw_mode;

#define icw_mode_mcs8085		0
#define icw_mode_8086			1

#define icw_mode_eoi_normal		0
#define icw_mode_eoi_auto		2

#define icw_mode_buffer_none		0
#define icw_mode_buffer_slave		0x08
#define icw_mode_buffer_master		0x0c

#define icw_mode_fully_nested_special		0x10
#define icw_mode_fully_nested_normal		0x00

__interrupt_h_export void interrupt_controller_initialize();
__interrupt_h_export void interrupt_controller_set(byte mask, byte enabled);
__interrupt_h_export void interrupt_controller_end(byte mask);


/*************************************************************************
 *			process_interrupt_stack_frame
 * @Name: process_interrupt_stack_frame
 * @Author: LuoHaoran
 * @Description: When a clock interrupt occurs, to enable scheduler to record
 * states of processes and schedule process, we push all current state into the
 * stack frame and call the scheduler.
**************************************************************************/

typedef struct __interrupt_stack_frame
{
	/** recorded by calling registers manually. **/
	selector ldt;
	word preserved_ldt;

	/** recorded by pushing registers into stack manually. **/
	selector es;
	word preserved_es;

	selector ds;
	word preserved_ds;

	selector fs;
	word preserved_fs;

	selector gs;
	word preserved_gs;

	/** recorded by calling 'pushad' **/
	dword edi;
	dword esi;
	dword ebp;
	dword reg_esp;
	dword ebx;
	dword edx;
	dword ecx;
	dword eax;

	/** record in order to be compatible with exception. **/
	dword error_code;

	dword eip;
	dword cs;
	dword eflags;
}
interrupt_stack_frame;

typedef void (*exception_handler)(dword vector, interrupt_stack_frame* stack_frame);
typedef void (*interrupt_handler)(interrupt_stack_frame* stack_frame);
__interrupt_h_export void interrupt_idt_initialize();
__interrupt_h_export void interrupt_set_exception_handler(dword vector, exception_handler handler);
__interrupt_h_export void interrupt_set_interrupt_handler(dword vector, interrupt_handler handler);

#define interrupt_vector_base		0x20
#define interrupt_ir0_clock		0x00
#define interrupt_ir1_keyboard		0x01
#define interrupt_ir2_slave		0x02
#define interrupt_ir3_serial2		0x03
#define interrupt_ir4_serial1		0x04
#define interrupt_ir5_lpt2		0x05
#define interrupt_ir6_floppy		0x06
#define interrupt_ir7_lpt2		0x07

#define interrupt_ir8_real_clock	0x08
#define interrupt_ir9_redirect		0x09
#define interrupt_ira_reserved		0x0a
#define interrupt_irb_reserved		0x0b
#define interrupt_irc_cursor		0x0c
#define interrupt_ird_fpu		0x0d
#define interrupt_ire_windchester	0x0e
#define interrupt_irf_reserved		0x0f

#endif
