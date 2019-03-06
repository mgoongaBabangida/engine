#pragma once
#include "MainContext.h"

class eInputController
{
public:
	eInputController(eMainContext* c):mainContext(c){}
	virtual void OnMouseMove(uint x, uint y);
	virtual void OnKeyPress(uint asci);
	virtual void OnMousePress(uint x, uint y, bool left);
	virtual void OnMouseRelease();

private:
	eMainContext* mainContext;
};
