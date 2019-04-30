#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include <SDL2-2.0.9/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL2-2.0.9/include/SDL_opengl.h>

#include "MainContext.h"
#include "InputController.h"
#include "Timer.h"

class IWindowImGui;

//***************************************
//dbGLWindowSDL
//---------------------------------------
class dbGLWindowSDL
{
public:
	dbGLWindowSDL();
	virtual ~dbGLWindowSDL();

	bool		InitializeGL();
	void		Run();

protected:
	std::unique_ptr<dbb::Timer>	dTimer;
	IWindowImGui*				guiWnd;
	eInputController			inputController;
	eMainContext				mainContext;

	SDL_Window*					window  = nullptr;

	const GLint					WIDTH	= 1200;
	const GLint					HEIGHT	= 600;

	void						paintGL();
};

#endif