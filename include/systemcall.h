#ifndef __system_call_h__
#define __system_call_h__

#ifdef __systemcall_impl__
	#define __systemcall_api	__public
#else
	#define __systemcall_api	extern
#endif

/** Trap Certain System Call Number To Get To A System Call. ***/
/** Pass The Sub Functin Number To The System Call By EAX Register ***/
/** Pass Other Parameters In The Order Of EDI, ESI, EDX, ECX, EBX **/

#define systemcall_stdout	0x90
#define stdout_putchar		0x00	/** EDI = CHAR TO PUT, ESI = COLOR OF THE CHAR **/
#define stdout_putstring	0x01	/** EDI = BEGIN OF STRING ADDRESS, ESI = COLOR OF THE STRING **/
#define stdout_clear		0x02	/** NO PARAM IS NEEDED	**/
#define stdout_cursor		0x03	/** EDI = THE ROW OF THE CURSOR, ESI = THE COLUMN OF THE CURSOR **/
#define stdout_putinteger	0x04	/** EDI = THE VALUE OF THE INTEGER **/

__systemcall_api void systemcall_stdout_initialize();

#define systemcall_process	0x93
#define process_yield		0x00
#define process_sleep		0x01	/** EDI = MILLS TO SLEEP **/
#define process_terminate	0x02
__systemcall_api void systemcall_process_initialize();

#define systemcall_semaphore	0x94
#define semaphore_init		0x00	/** EDI = SEMAPHORE_ID, ESI = INITIAL_VALUE **/
#define semaphore_p		0x01	/** EDI = SEMAPHORE_ID **/
#define semaphore_v		0x02	/** EDI = SEMAPHORE_ID **/
__systemcall_api void systemcall_semaphore_initialize();

#endif
