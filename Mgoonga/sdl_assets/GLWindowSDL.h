#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include <SDL2-2.0.9/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL2-2.0.9/include/SDL_opengl.h>

#include "sdl_assets.h"

#include <base/InputController.h>
#include <math/Timer.h>

class eMainContextBase;
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

protected:
	std::unique_ptr<math::Timer>		dTimer;
	IWindowImGui*						guiWnd;
	eInputController					inputController;
	std::unique_ptr<eMainContextBase>	mainContext; 

	SDL_Window*							window  = nullptr;

	const GLint							WIDTH	= 1200;
	const GLint							HEIGHT	= 600;

	void								PaintGL();
};

#endif