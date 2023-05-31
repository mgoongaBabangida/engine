#include "stdafx.h"
#include "InputController.h"

//------------------------------------------------------------------
void eInputController::OnMouseMove(uint32_t x, uint32_t y)
{
	bool taken = false;
	for(auto& observer : observersPriority)
	{
		if(observer && observer->OnMouseMove(x, y))
		{
			taken = true;
			break;
		}
	}

	if(!taken)
	{
		for(auto& observer : observers)
		{
			if (observer)
				observer->OnMouseMove(x, y);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseMove(x, y);
	}
}

//------------------------------------------------------------------
bool eInputController::OnKeyPress(uint32_t asci)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer)
		{
			if(observer->OnKeyPress(asci))
				return false;
			taken = false; //do not take keyboard, only mouse
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
      if (observer)
        if (observer->OnKeyPress(asci))
          break;
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			if (observer->OnKeyPress(asci))
				break;
	}

	return true;
}

//----------------------------------------------------------------------
void eInputController::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMousePress(x,y,left))
		{
			taken = true;
			break;
		}
	}
	if (!taken)
	{
		for (auto& observer : observers)
		{
			if (observer)
				observer->OnMousePress(x, y, left);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMousePress(x, y, left);
	}
}

//----------------------------------------------------------------------
void eInputController::OnMouseWheel(int32_t _x, int32_t _y)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMouseWheel(_x, _y))
		{
			taken = true;
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
			if (observer)
				observer->OnMouseWheel(_x, _y);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseWheel(_x, _y);
	}
}

//----------------------------------------------------------------------
void eInputController::OnMouseRelease()
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMouseRelease())
		{
			taken = true;
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
			if (observer)
				observer->OnMouseRelease();
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseRelease();
	}
}

//---------------------------------------------------------------------------
void eInputController::AddObserver(IInputObserver* _obs, ePriority _priority)
{
	switch(_priority)
	{
		case MONOPOLY:	observersPriority.push_back(_obs);	break;
		case STRONG:		observers.push_front(_obs);					break;
		case WEAK:			observers.push_back(_obs);					break;
		case ALWAYS:		observersAlways.push_back(_obs);		break;
	}
}
