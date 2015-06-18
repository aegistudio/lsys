#ifndef __system_call_h__
#define __system_call_h__

#define system_call_stdout	0x21
#define stdout_putchar		0x00
#define stdout_putstring	0x01
#define stdout_clear		0x02
#define stdout_cursor		0x03

#define system_call_stdin	0x22

#define system_call_process	0x23
#define process_yield		0x00
#define process_sleep		0x01
#define process_terminate	0x02

#endif
