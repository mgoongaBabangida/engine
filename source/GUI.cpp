#include "stdafx.h"
#include "GUI.h"

bool GUI::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	if(left && isVisible && isPressed(x, y))
	{
		Perssed();
		return true;
	}
	return false;
}
