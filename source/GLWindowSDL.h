#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
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
	//Events
	//virtual void mouseMoveEvent(QMouseEvent* e)		override;
	//virtual void keyPressEvent(QKeyEvent* e)		override;
	//virtual void mousePressEvent(QMouseEvent * e)	override;
	//virtual void mouseReleaseEvent(QMouseEvent * e) override;

protected:
	std::unique_ptr<dbb::Timer>		dTimer;
	eMainContext					mainContext;
	eInputController				inputController;

	SDL_Window*						window  = nullptr;
	//SDL_GLContext					context;

	const GLint						WIDTH	= 1200;
	const GLint						HEIGHT	= 600;

	void							paintGL();
	void							updateSharedData();
};

#endif