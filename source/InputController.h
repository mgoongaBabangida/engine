#pragma once
#include <deque>
#include <vector>

class IInputObserver;

enum ePriority
{
	WEAK,
	STRONG,
	MONOPOLY,
};

class eInputController
{
public:
	eInputController() = default;
	virtual void OnMouseMove(uint32_t x, uint32_t y);
	virtual void OnKeyPress(uint32_t asci);
	virtual void OnMousePress(uint32_t x, uint32_t y, bool left);
	virtual void OnMouseRelease();
	void		 AddObserver(IInputObserver*, ePriority);

private:
	std::vector<IInputObserver*>	observersPriority;
	std::deque<IInputObserver*>		observers;
};
