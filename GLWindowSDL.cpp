#include "GLWindowSDL.h"
#include "SharedData.h"

SDL_GLContext					context;

dbGLWindowSDL::dbGLWindowSDL()
	:inputController()
	,mainContext(&inputController, "Resources/", "assets/", "shaders/")
{}

dbGLWindowSDL::~dbGLWindowSDL()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
////======================================
////dbGLWindowSDL::initializeGL
////---------------------------------------
bool dbGLWindowSDL::InitializeGL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window	= SDL_CreateWindow("OpenGl", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if(window == nullptr)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;

	auto res = glewInit();
	if(GLEW_OK != res)
	{
		return false;
	}
	mainContext.InitializeGL();
	SDL_GL_MakeCurrent(window, NULL);
	dTimer.reset(new dbb::Timer([this]()->bool
								{
									this->paintGL(); return true;
								}));
	dTimer->start(100);
	return true;
}
//======================================
//dbGLWindowSDL::Run
//---------------------------------------
void dbGLWindowSDL::Run()
{
	SDL_Event windowEvent;
	while(true)
	{
		if(SDL_PollEvent(&windowEvent))
		{
			if(SDL_QUIT == windowEvent.type)
			{
				break;
			}
			else if(SDL_KEYDOWN == windowEvent.type)
			{
				inputController.OnKeyPress(windowEvent.key.keysym.sym);
			}
			else if(SDL_MOUSEBUTTONDOWN == windowEvent.type)
			{
				inputController.OnMousePress(windowEvent.button.x,
											 windowEvent.button.y, 
											 windowEvent.button.button == SDL_BUTTON_LEFT);
			}
			else if(SDL_MOUSEBUTTONUP == windowEvent.type)
			{
				inputController.OnMouseRelease();
			}
			else if(SDL_MOUSEMOTION == windowEvent.type)
			{
				inputController.OnMouseMove(windowEvent.motion.x, windowEvent.motion.y);
			}
		}
	}
}
//======================================
//dbGLWindowSDL::paintGL
//---------------------------------------
void dbGLWindowSDL::paintGL()
{
	static bool flag = false;
	if(!flag)
	{
		SDL_GL_MakeCurrent(window, context);
		flag = true;
	}
	mainContext.PaintGL();
	SDL_GL_SwapWindow(window);
}
//======================================
//dbGLWindowSDL::updateSharedData
//---------------------------------------
void dbGLWindowSDL::updateSharedData()
{
	//mainContext.UpdateLight(m_sData->vec_data[0].x, m_sData->vec_data[0].y, m_sData->vec_data[0].z);
}