#ifndef __video_h__
#define __video_h__

#include "define.h"

#ifdef __video_c__
	#define __video_h_export __public
#else 
	#define __video_h_export extern
#endif

__constant(video_line_length	,	80);
__constant(video_line_count	,	50);
__constant(video_alignment_left	,	0);
__constant(video_alignment_mid	,	1);
__constant(video_alignment_right,	2);

__video_h_export void video_set_cursor		(dword line, dword column);
__video_h_export void video_clear_screen	();
//__video_h_export void video_put_char		(byte character, byte color);
//__video_h_export void video_put_string	(byte* string, byte color, dword alignment);
//__video_h_export void video_line_move		(dword offset);
//__video_h_export void video_column_move	(dword offset);

#endif
