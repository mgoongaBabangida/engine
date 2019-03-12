#pragma once
#include "MainContext.h"

class eInputController
{
public:
	eInputController(eMainContext* c):mainContext(c){}
	virtual void OnMouseMove(uint32_t x, uint32_t y);
	virtual void OnKeyPress(uint32_t asci);
	virtual void OnMousePress(uint32_t x, uint32_t y, bool left);
	virtual void OnMouseRelease();

private:
	eMainContext* mainContext;
};
