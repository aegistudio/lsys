#ifndef __video_c__
#define __video_c__

#include "define.h"
#include "video.h"

__public void kernel_video_setup()
{
	video_clear_screen();
	video_set_cursor(0, 0);
}

#endif
