//This file contains the 'main' function. Program execution begins and ends there.

#include "stdafx.h"

#include <base/InputController.h>
#include <sdl_assets/GLWindowSDL.h>

#include "MainContext.h"
#include "SandBoxGame.h"
#include "AmericanTreasureGame.h"

class GameFactory :  public IGameFactory
{
public:
	eMainContextBase* CreateGame(eInputController* _input,
		                         std::vector<IWindowImGui*> _imgui_windows) const
	{
		return new eMainContext(_input, _imgui_windows, "Resources/", "assets/", "shaders/");
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

//https://www.youtube.com/watch?v=v9edDckn6m8

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
