#ifndef __video_c__
#define __video_c__

#include "driver/video.h"

extern void asm_video_brush_screen(dword and, dword or, dword begin_index, dword end_index);
extern void asm_video_put_char(dword index, dword sequence);

__private dword video_line_counter;
__private dword video_column_counter;

#define video_table_size	4

dword video_get_line()
{
	return video_line_counter;
}

dword video_get_column()
{
	return video_column_counter;
}

void video_clear_screen()
{
	video_line_counter = 0;
	video_column_counter = 0;
	asm_video_brush_screen(0, 0, 0, video_line_length * video_line_count * 2);
}

void video_set_cursor(dword line, dword column)
{
	video_column_counter = column % video_line_length;
	video_line_counter = (line + column / video_line_length) 
		% video_line_count;
}

void video_put_char(byte character, byte color)
{
	dword index = video_line_counter * video_line_length + video_column_counter;
	asm_video_put_char(index * 2, character | color << 8);
	video_set_cursor(video_line_counter, video_column_counter + 1);
}

void video_brush_screen(dword and, dword or, dword begin_index, dword end_index)
{
	asm_video_brush_screen(and, or, begin_index * 2, end_index * 2);
}

void video_put_string(byte* string, byte color)
{
	byte* pointer = string;
	while(*pointer != 0)
	{
		if(*pointer >= ' ')
			video_put_char(*pointer, color);
		else if(*pointer == '\n')
			video_set_cursor(video_line_counter + 1, 0);
		else if(*pointer == '\t')
		{
			int i = 0;
			for(; i < video_table_size; i ++)
				video_put_char(' ', color);
		}
		pointer ++;
	}
}

void video_line_move		(dword offset)
{
	video_set_cursor(video_line_counter + 1, video_column_counter);
}

void video_column_move		(dword offset)
{
	video_set_cursor(video_line_counter, video_column_counter + 1);
}
#endif
