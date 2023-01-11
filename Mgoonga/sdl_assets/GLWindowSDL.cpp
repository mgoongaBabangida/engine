#include "stdafx.h"
#include "GLWindowSDL.h"

#include "ImGuiContext.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include <iostream>

#include "MainContextBase.h"
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

SDL_GLContext					context;

//***************************************
//dbGLWindowSDL::~dbGLWindowSDL
//---------------------------------------
dbGLWindowSDL::dbGLWindowSDL(const IGameFactory& _factory)
: inputController()
{
  guiWnd.push_back(new eWindowImGui("Gui"));
  guiWnd.push_back(new eWindowImGui("Debug"));
  guiWnd.push_back(new eWindowImGui("Object"));

  on_close = std::function<void()>{ [this](){this->Close(); } };
  guiWnd[0]->Add(MENU, "Close", reinterpret_cast<void*>(&on_close));

	mainContext.reset(_factory.CreateGame(&inputController, guiWnd));
}

//======================================
//dbGLWindowSDL::~dbGLWindowSDL
//---------------------------------------
dbGLWindowSDL::~dbGLWindowSDL()
{
  dTimer->stop();
	eImGuiContext::GetInstance(&context, window).CleanUp();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
  for(auto* gui : guiWnd)
	  delete gui;
}

//======================================
//dbGLWindowSDL::initializeGL
//---------------------------------------
bool dbGLWindowSDL::InitializeGL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	
	window	= SDL_CreateWindow("OpenGl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	
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
	eImGuiContext::GetInstance(&context, window).Init();

	mainContext->InitializeGL();
	SDL_GL_MakeCurrent(window, NULL);
	dTimer.reset(new math::Timer([this]()->bool
								{
									this->PaintGL();
									return true;
								}));
	dTimer->start(15); //~70 fps
	return true;
}
//======================================
//dbGLWindowSDL::Run
//---------------------------------------
void dbGLWindowSDL::Run()
{
	SDL_Event windowEvent;
	while(running)
	{
		if(SDL_PollEvent(&windowEvent))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui_ImplSDL2_ProcessEvent(&windowEvent);
			if (io.WantCaptureMouse)
				continue;
      if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				dTimer->stop();
        running = false;
				break;
			}
			else if(SDL_KEYDOWN == windowEvent.type)
			{
				if (!inputController.OnKeyPress(windowEvent.key.keysym.sym))
					return;
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
//dbGLWindowSDL::Close
//---------------------------------------
void dbGLWindowSDL::Close()
{
  running = false;
}

//======================================
//dbGLWindowSDL::paintGL
//---------------------------------------
void dbGLWindowSDL::PaintGL()
{
	static bool flag = false;
	if(!flag)
	{
		SDL_GL_MakeCurrent(window, context);
		//SDL_GL_SetSwapInterval(1);
		flag = true;
	}
	
	eImGuiContext::GetInstance(&context, window).NewFrame();

	/*eWindowImGuiDemo demo;
  demo.Render();*/

  for(auto* gui : guiWnd)
    gui->Render();

	eImGuiContext::GetInstance(&context, window).PreRender();
	mainContext->PaintGL();
	eImGuiContext::GetInstance(&context, window).Render();

	SDL_GL_SwapWindow(window);
}
