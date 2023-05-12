#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include <SDL/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL/include/SDL_opengl.h>

#include "sdl_assets.h"

#include <base/InputController.h>
#include <math/Timer.h>

class IGame;
class IGameFactory;
class IWindowImGui;

//***************************************
//dbGLWindowSDL
//---------------------------------------
class DLL_SDL_ASSETS dbGLWindowSDL
{
public:
	dbGLWindowSDL(const IGameFactory&);
	~dbGLWindowSDL();

	bool		InitializeGL();
	void		Run();
  void		Close();
	void		OnDockSpace();

protected:
	std::unique_ptr<math::Timer>		dTimer;
	std::vector<IWindowImGui*>			guiWnd;
	eInputController								inputController;
	std::unique_ptr<IGame>					mainContext; 

	SDL_Window*							window  = nullptr;

	const GLint							WIDTH	= 1200; //@todo
	const GLint							HEIGHT	= 600;

  bool                    running = true;
  std::function<void()>		on_close;

	void										PaintGL(); //update? tick?
};

//----------------------------------------------------------------------
class IGameFactory
{
public:
	virtual IGame* CreateGame(eInputController*						_input,
														std::vector<IWindowImGui*>	_imgui_windows) const = 0;
};

#endif