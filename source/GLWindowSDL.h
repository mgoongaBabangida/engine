#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include "MainContext.h"
#include "InputController.h"
#include "Timer.h"

class dbGLWindowSDL
{
public:
	dbGLWindowSDL();
	virtual ~dbGLWindowSDL();

	bool		InitializeGL();
	void		Run();

protected:
	std::unique_ptr<dbb::Timer>		dTimer;
	eMainContext					mainContext;
	eInputController				inputController;

	SDL_Window*						window  = nullptr;

	const GLint						WIDTH	= 1200;
	const GLint						HEIGHT	= 600;

	void							paintGL();
	void							updateSharedData();
};

#endif