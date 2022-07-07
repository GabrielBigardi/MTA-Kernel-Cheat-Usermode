#include "memory.h"
#pragma once

namespace draw
{
	bool box(int x, int y, int w, int h, int t, int r, int g, int b)
	{
		NULL_MEMORY instructions;
		instructions.write = FALSE;
		instructions.read = FALSE;
		instructions.req_base = FALSE;
		instructions.draw_box = TRUE;

		instructions.x = x;
		instructions.y = y;
		instructions.w = w;
		instructions.h = h;
		instructions.t = t;

		instructions.r = r;
		instructions.g = g;
		instructions.b = b;


		call_hook(&instructions);

		return true;
	}
}