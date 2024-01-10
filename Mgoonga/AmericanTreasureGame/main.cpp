//This file contains the 'main' function. Program execution begins and ends there.

#include "stdafx.h"

#include <base/InputController.h>
#include <sdl_assets/GLWindowSDL.h>

#include "AmericanTreasureGame.h"

//------------------------------------------
class GameFactory :  public IGameFactory
{
public:
	eMainContextBase* CreateGame(eInputController* _input,
		                         std::vector<IWindowImGui*>& _imgui_windows) const
	{
		return new AmericanTreasureGame(_input, _imgui_windows, "../game_assets/Resources/", "../game_assets/assets/", "../game_assets/shaders/");
	}
};

int main(int argc, char* argv[])
{
	GameFactory factory;
	dbGLWindowSDL wnd(factory);
	if(wnd.InitializeGL())
	{
		wnd.Run();
	}
	return 0;
}
