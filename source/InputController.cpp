#include "stdafx.h"
#include "InputController.h"
#include "InterfacesDB.h"

void eInputController::OnMouseMove(uint32_t x, uint32_t y)
{
	bool taken = false;
	for(auto observer : observersPriority)
	{
		if(observer && observer->OnMouseMove(x, y))
		{
			taken = true;
			break;
		}
	}
	if(!taken)
	{
		for(auto observer : observers)
		{
			if (observer)
				observer->OnMouseMove(x, y);
		}
	}
}

void eInputController::OnKeyPress(uint32_t asci)
{
	bool taken = false;
	for (auto observer : observersPriority)
	{
		if (observer && observer->OnKeyPress(asci))
		{
			taken = true;
			break;
		}
	}
	if (!taken)
	{
		for (auto observer : observers)
		{
			if (observer)
				observer->OnKeyPress(asci);
		}
	}
}

void eInputController::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	bool taken = false;
	for (auto observer : observersPriority)
	{
		if (observer && observer->OnMousePress(x,y,left))
		{
			taken = true;
			break;
		}
	}
	if (!taken)
	{
		for (auto observer : observers)
		{
			if (observer)
				observer->OnMousePress(x, y, left);
		}
	}
}

void eInputController::OnMouseRelease()
{
	bool taken = false;
	for (auto observer : observersPriority)
	{
		if (observer && observer->OnMouseRelease())
		{
			taken = true;
			break;
		}
	}
	if (!taken)
	{
		for (auto observer : observers)
		{
			if (observer)
				observer->OnMouseRelease();
		}
	}
}

void eInputController::AddObserver(IInputObserver* _obs, ePriority _priority)
{
	switch(_priority)
	{
		case MONOPOLY:	observersPriority.push_back(_obs);	break;
		case STRONG:	observers.push_front(_obs);			break;
		case WEAK:		observers.push_back(_obs);			break;
	}
}
