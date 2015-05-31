#ifndef __video_c__
#define __video_c__

#include "define.h"
#include "video.h"

__public void kernel_video_setup()
{
	video_clear_screen();
	video_set_cursor(0, 0);
	char print = 'a';
	for(; print <= 'z'; print ++) video_put_char(print, 0x07);
	video_brush_screen(0xffffffff, 0x01001800, 0, video_line_length * video_line_count);
	video_put_string("hoqweuoqwieuqwidcxkhsalkdjsadjslakjdksajdk\nalwjsldjs\tA1oiaeuqwoieuiqwoeuoqwueoqweuwqiuo", 0x28);
}

#endif
