#pragma once

#include <deque>
#include <vector>

#include "interfaces.h"

//-------------------------------------------------------
class DLL_BASE eInputController
{
public:
	eInputController() = default;
	virtual void OnMouseMove(uint32_t x, uint32_t y, KeyModifiers);
	virtual bool OnKeyPress(uint32_t asci, KeyModifiers _modifier);
	virtual void OnMousePress(uint32_t x, uint32_t y, bool left , KeyModifiers = KeyModifiers::NONE);
	virtual void OnMouseRelease(KeyModifiers = KeyModifiers::NONE);
	virtual void OnMouseWheel(int32_t x, int32_t y, KeyModifiers = KeyModifiers::NONE);

	void				 AddObserver(IInputObserver*, ePriority);
	void				 DeleteObserver(IInputObserver*);

private:
	std::vector<IInputObserver*>	observersPriority;
	std::deque<IInputObserver*>		observers;
	std::vector<IInputObserver*>	observersAlways;
};
