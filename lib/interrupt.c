#ifndef __interrupt_c__
#define __interrupt_c__

#include "interrupt.h"

extern void asm_interrupt_outb(word port, byte word);
extern byte asm_interrupt_inb(word port);

void interrupt_controller_initialize()
{
	
}

#endif
