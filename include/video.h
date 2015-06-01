#ifndef __video_h__
#define __video_h__

#include "define.h"

#ifdef __video_c__
	#define __video_h_export __public
#else 
	#define __video_h_export extern
#endif

#define video_line_length		80
#define video_line_count		25
#define video_alignment_left		0
#define video_alignment_mid		1
#define video_alignment_right		2

__video_h_export void video_set_cursor		(dword line, dword column);
__video_h_export dword video_get_line		();
__video_h_export dword video_get_column		();
__video_h_export void video_clear_screen	();
__video_h_export void video_brush_screen	(dword and, dword or, dword begin_index, dword end_index);
__video_h_export void video_put_char		(byte character, byte color);
__video_h_export void video_put_string		(byte* string, byte color);
__video_h_export void video_line_move		(dword offset);
__video_h_export void video_column_move		(dword offset);

#endif
