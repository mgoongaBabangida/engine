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
	dbGLWindowSDL(const IGameFactory&, GLint _width, GLint _height);
	~dbGLWindowSDL();

	bool	InitializeGL();
	void	Run();
	void	Close();
	void	OnDockSpace();

protected:
	void	PaintGL(); //update? tick?

	std::unique_ptr<math::Timer>	dTimer;
	std::vector<IWindowImGui*>		guiWnd;
	eInputController							inputController;
	std::unique_ptr<IGame>				mainContext; 

	SDL_Window*						window  = nullptr;

	/*const*/ GLint						WIDTH		= 1200;
	/*const*/ GLint						HEIGHT	= 750;

	bool							running = true;
	bool							m_disable_system_cursor_under_view = true;
	bool							m_vsync = true;
	unsigned int			m_min_frametime = 15;  //~70 fps

	std::function<void()>			on_close;
};

//----------------------------------------------------------------------
class IGameFactory
{
public:
	virtual IGame* CreateGame(eInputController*				_input,
									std::vector<IWindowImGui*>&	_imgui_windows, int _width, int _height) const = 0;
};

#endif