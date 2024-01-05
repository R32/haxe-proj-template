#include <string.h>
#include "page_pic.h"
#include "tonc.h"

COLOR *vid_flip()
{
	vid_page= (COLOR*)((u32)vid_page ^ VRAM_PAGE_SIZE);
	REG_DISPCNT ^= DCNT_PAGE;	// update control register	
	return vid_page;
}

void load_gfx()
{
	int ii;
	// (1) Because my bitmaps here don't fit the  screen size,
	// I'll have to load them one scanlline at a time
	for(ii=0; ii<16; ii++) {
		memcpy(&vid_mem_front[ii*120], &frontBitmap[ii*144/4], 144);
		memcpy(&vid_mem_back[ii*120], &backBitmap[ii*144/4], 144);
	}

	// (2) You don't have to do everything with memcpy.
	// In fact, for small blocks it might be better if you didn't.
	// Just mind your types, though. No sense in copying from a 32bit
	// array to a 16bit one.
	u32 *dst= (u32*)pal_bg_mem;
	for(ii=0; ii<8; ii++)
		dst[ii]= frontPal[ii];
}

int main()
{
	load_gfx();

	// Set video mode to 4 (8bpp, 2 pages)
	REG_DISPCNT= DCNT_MODE4 | DCNT_BG2;

	int ii=0;

	while(1) {
		while(KEY_DOWN_NOW(KEY_START)); // pause with start

		vid_vsync();

		// (3) Count 60 frames, then flip pages
		if(++ii == 60) {
			ii=0;
			vid_flip();
		}
	}
	return 0;
}
