#pragma once

#include <deque>
#include <vector>

#include "interfaces.h"

enum ePriority
{
	WEAK,
	STRONG,
	MONOPOLY,
};

//-------------------------------------------------------
class DLL_BASE eInputController
{
public:
	eInputController() = default;
	virtual void OnMouseMove(uint32_t x, uint32_t y);
	virtual bool OnKeyPress(uint32_t asci);
	virtual void OnMousePress(uint32_t x, uint32_t y, bool left);
	virtual void OnMouseRelease();
	void		 AddObserver(IInputObserver*, ePriority);

private:
	std::vector<IInputObserver*>	observersPriority;
	std::deque<IInputObserver*>		observers;
};
