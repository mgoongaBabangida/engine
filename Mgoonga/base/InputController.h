#pragma once

#include <deque>
#include <vector>

#include "interfaces.h"

//-------------------------------------------------------
class DLL_BASE eInputController
{
public:
	eInputController() = default;
	virtual void OnMouseMove(uint32_t x, uint32_t y);
	virtual bool OnKeyPress(uint32_t asci);
	virtual void OnMousePress(uint32_t x, uint32_t y, bool left);
	virtual void OnMouseRelease();
	virtual void OnMouseWheel(int32_t x, int32_t y);
	void				 AddObserver(IInputObserver*, ePriority);
	void				 DeleteObserver(IInputObserver*);

private:
	std::vector<IInputObserver*>	observersPriority;
	std::deque<IInputObserver*>		observers;
	std::vector<IInputObserver*>	observersAlways;
};
