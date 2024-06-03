#include "stdafx.h"
#include "InputController.h"

eInputController::eInputController()
{
	for(int i = 0; i < 128; ++i)
	{
		keysPressed.push_back(false);
	}
}

//------------------------------------------------------------------
void eInputController::OnMouseMove(uint32_t x, uint32_t y, KeyModifiers _modifiers)
{
	bool taken = false;
	for(auto& observer : observersPriority)
	{
		if(observer && observer->OnMouseMove(x, y, _modifiers))
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
				observer->OnMouseMove(x, y, _modifiers);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseMove(x, y, _modifiers);
	}
}

//------------------------------------------------------------------
bool eInputController::OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier)
{
	if (_asci > keysPressed.size())
		return true;

	keysPressed[_asci] = true;
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer)
		{
			if(observer->OnKeyJustPressed(_asci, _modifier))
				return false;
			taken = false; //do not take keyboard, only mouse ?
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
			if(observer)
			{
				if (observer->OnKeyJustPressed(_asci, _modifier)) break;
			}
		}
	}

	for (auto& observer : observersAlways)
	{
		if(observer)
		{
			if (observer->OnKeyJustPressed(_asci, _modifier)) break;
		}
	}

	return true;
}

//------------------------------------------------------------------
bool eInputController::OnKeyPress()
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer)
		{
			if(observer->OnKeyPress(keysPressed, KeyModifiers::NONE))
				return false;
			taken = false; //do not take keyboard, only mouse ?
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
			if(observer)
			{
				if (observer->OnKeyPress(keysPressed, KeyModifiers::NONE)) break;
			}
		}
	}

	for (auto& observer : observersAlways)
	{
		if(observer)
		{
			if (observer->OnKeyPress(keysPressed, KeyModifiers::NONE)) break;
		}
	}

	return true;
}

//----------------------------------------------------------------------
bool eInputController::OnKeyRelease(uint32_t _asci)
{
	if (_asci > keysPressed.size())
		return true;

	if(keysPressed[_asci])
	{
		keysPressed[_asci] = false;
	}
	
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer)
		{
			if (observer->OnKeyRelease((ASCII)_asci, keysPressed, KeyModifiers::NONE))
				return false;
			taken = false; //do not take keyboard, only mouse ?
			break;
		}
	}

	if (!taken)
	{
		for (auto& observer : observers)
		{
			if (observer)
			{
				if (observer->OnKeyRelease((ASCII)_asci, keysPressed, KeyModifiers::NONE)) break;
			}
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
		{
			if (observer->OnKeyRelease((ASCII)_asci, keysPressed, KeyModifiers::NONE)) break;
		}
	}

	return true;
}

//----------------------------------------------------------------------
void eInputController::OnMousePress(uint32_t x, uint32_t y, bool left, KeyModifiers _modifier)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMousePress(x,y,left, _modifier))
		{
			taken = true;
			break;
		}
	}
	if (!taken)
	{
		for (auto& observer : observers)
		{
			if (observer && observer->OnMousePress(x, y, left, _modifier))
			{
				taken = true;
				break;
			}
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMousePress(x, y, left, _modifier);
	}
}

//----------------------------------------------------------------------
void eInputController::OnMouseWheel(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMouseWheel(_x, _y, _modifier))
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
				observer->OnMouseWheel(_x, _y, _modifier);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseWheel(_x, _y, _modifier);
	}
}

//----------------------------------------------------------------------
bool eInputController::IsAnyKeyPressed() const
{
	for(bool isPressed : keysPressed)
	{
		if(isPressed) return true;
	}
	return false;
}

//----------------------------------------------------------------------
void eInputController::OnMouseRelease(KeyModifiers _modifier)
{
	bool taken = false;
	for (auto& observer : observersPriority)
	{
		if (observer && observer->OnMouseRelease(_modifier))
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
				observer->OnMouseRelease(_modifier);
		}
	}

	for (auto& observer : observersAlways)
	{
		if (observer)
			observer->OnMouseRelease(_modifier);
	}
}

//---------------------------------------------------------------------------
void eInputController::AddObserver(IInputObserver* _obs, ePriority _priority)
{
	//@todo check if it is already there
	switch(_priority)
	{
		case MONOPOLY:	observersPriority.push_back(_obs);	break;
		case STRONG:		observers.push_front(_obs);					break;
		case WEAK:			observers.push_back(_obs);					break;
		case ALWAYS:		observersAlways.push_back(_obs);		break;
	}
}

//---------------------------------------------------------------------------
void eInputController::DeleteObserver(IInputObserver* _obs)
{
	if(auto it = std::remove(observersPriority.begin(), observersPriority.end(), (_obs)); it!= observersPriority.end())
		observersPriority.erase(it);
	else if(auto it = std::remove(observers.begin(), observers.end(), (_obs)); it != observers.end())
		observers.erase(it);
	else if (auto it = std::remove(observersAlways.begin(), observersAlways.end(), (_obs)); it != observersAlways.end())
		observersAlways.erase(it);
}
