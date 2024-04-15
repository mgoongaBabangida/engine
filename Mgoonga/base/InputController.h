#pragma once

#include <deque>
#include <vector>

#include "interfaces.h"

//-------------------------------------------------------
class DLL_BASE eInputController
{
public:
	eInputController();
	virtual void	OnMouseMove(uint32_t x, uint32_t y, KeyModifiers);
	virtual bool	OnKeyJustPressed(uint32_t asci, KeyModifiers _modifier = KeyModifiers::NONE);
	virtual bool	OnKeyPress();
	virtual bool	OnKeyRelease(uint32_t asci);
	virtual void	OnMousePress(uint32_t x, uint32_t y, bool left , KeyModifiers = KeyModifiers::NONE);
	virtual void	OnMouseRelease(KeyModifiers = KeyModifiers::NONE);
	virtual void	OnMouseWheel(int32_t x, int32_t y, KeyModifiers = KeyModifiers::NONE);
	
	bool			IsAnyKeyPressed() const;

	void			AddObserver(IInputObserver*, ePriority);
	void			DeleteObserver(IInputObserver*);

private:
	std::vector<IInputObserver*>	observersPriority;
	std::deque<IInputObserver*>		observers;
	std::vector<IInputObserver*>	observersAlways;

	std::vector<bool>				keysPressed;
};
