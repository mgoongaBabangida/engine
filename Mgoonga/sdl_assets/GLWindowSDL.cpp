#include "stdafx.h"
#include "GLWindowSDL.h"

#ifndef STANDALONE
#include "ImGuiContext.h"
#include "ImGuiWindowExternal.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#endif

#include"Windows_Related_API.h"

#include <base/interfaces.h>
#include <base/Object.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include "ImGuizmo.h"

#include <glm/glm/gtx/matrix_decompose.hpp>

SDL_GLContext					context;
ImVec2								viewport_offset;

#ifndef STANDALONE
const unsigned int ENGINE_CONTROLS_SPACE_X = 575;
const unsigned int ENGINE_CONTROLS_SPACE_Y = 125;
#else
const unsigned int ENGINE_CONTROLS_SPACE_X = 0;
const unsigned int ENGINE_CONTROLS_SPACE_Y = 0;
#endif

const int mysterious_y_border_offset = 20; //@todo

//***************************************
//dbGLWindowSDL::dbGLWindowSDL
//---------------------------------------
dbGLWindowSDL::dbGLWindowSDL(const IGameFactory& _factory, GLint _width, GLint _height)
: inputController(), WIDTH(_width), HEIGHT(_height)
{
#ifndef STANDALONE
  guiWnd.push_back(new eWindowImGuiExternal("Lights & Cameras"));//0
  guiWnd.push_back(new eWindowImGuiExternal("Pipeline"));//1
  guiWnd.push_back(new eWindowImGuiExternal("Object Transform"));//2
	guiWnd.push_back(new eWindowImGuiExternal("Shader"));//3
	guiWnd.push_back(new eMainImGuiWindow());//4
	guiWnd.push_back(new eWindowImGuiExternal("Create"));//5
	guiWnd.push_back(new eWindowImGuiExternal("Objects list"));//6
	guiWnd.push_back(new eWindowImGuiExternal("Object Material"));//7
	guiWnd.push_back(new eWindowImGuiExternal("Object Rigger"));//8
	guiWnd.push_back(new eWindowImGuiExternal("Console"));//9
	guiWnd.push_back(new eWindowImGuiExternal("Particle System Tool"));//10
	guiWnd.push_back(new eWindowImGuiExternal("Terrain Generation Tool"));//11
	guiWnd.push_back(new eWindowImGuiExternal("Game Debug"));//12
	guiWnd.push_back(new eWindowImGuiExternal("Physics Engine Test"));//13
	guiWnd.push_back(new eWindowImGuiExternal("HDR BLOOM"));//14
	guiWnd.push_back(new eWindowImGuiExternal("Clouds Generation Tool"));//15

  on_close = std::function<void()>{ [this](){this->Close(); } };
  guiWnd[4]->Add(MENU, "Close", reinterpret_cast<void*>(&on_close));

	guiWnd[ExternalWindow::PIPELINE_WND]->Add(CHECKBOX, "Disable system cursor", &m_disable_system_cursor_under_view);
	guiWnd[ExternalWindow::PIPELINE_WND]->Add(CHECKBOX, "V-sync", &m_vsync);
	static std::function<void(int, int*&)> min_frametime_callback = [this](int rate, int*& _val)
	{
		static bool first_call = true;
		if (first_call)
		{
			*_val = m_min_frametime;
			first_call = false;
			return;
		}
		m_min_frametime = (unsigned int)rate;
	};
	guiWnd[ExternalWindow::PIPELINE_WND]->Add(SPIN_BOX, "Min frametime", (void*)&min_frametime_callback);
#endif

	mainContext.reset(_factory.CreateGame(&inputController, guiWnd, WIDTH, HEIGHT));
}

//======================================
//dbGLWindowSDL::~dbGLWindowSDL
//---------------------------------------
dbGLWindowSDL::~dbGLWindowSDL()
{
  dTimer->stop();
#ifndef STANDALONE
	eImGuiContext::GetInstance(&context, window).CleanUp();
#endif
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
		return false;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	
	window	= SDL_CreateWindow("Mgoonga", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH + ENGINE_CONTROLS_SPACE_X, HEIGHT + ENGINE_CONTROLS_SPACE_Y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_ALLOW_HIGHDPI);
	
	if(m_vsync)
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
#ifndef STANDALONE
	eImGuiContext::GetInstance(&context, window).Init();
#endif
	mainContext->InitializeGL();
	ImGuizmo::AllowAxisFlip(false);

	SDL_GL_MakeCurrent(window, NULL);

	dTimer.reset(new math::Timer([this]()->bool
								{
									this->PaintGL();
									return true;
								}));
	dTimer->start(m_min_frametime);
	return true;
}

//======================================
//dbGLWindowSDL::Run
//---------------------------------------
void dbGLWindowSDL::Run()
{
	if (m_vsync)
		SDL_GL_SetSwapInterval(1);
	else
		SDL_GL_SetSwapInterval(0);

	SDL_Event windowEvent;
	while(running)
	{
		if(SDL_PollEvent(&windowEvent))
		{
#ifndef STANDALONE
			ImGuiIO& io = ImGui::GetIO();
			ImGui_ImplSDL2_ProcessEvent(&windowEvent);
			/*if (io.WantCaptureMouse)
				continue;*/
			if (eImGuiContext::BlockEvents())
				continue;
#endif			

			KeyModifiers mod = KeyModifiers::NONE;
			SDL_Keymod state = SDL_GetModState();
			if ((SDL_GetModState() & KMOD_SHIFT) && (SDL_GetModState() & KMOD_CTRL))
				mod = KeyModifiers::CTRL_SHIFT;
			else if ((SDL_GetModState() & KMOD_SHIFT))
				mod = KeyModifiers::SHIFT;
			else if ((SDL_GetModState() & KMOD_CTRL))
				mod = KeyModifiers::CTRL;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				dTimer->stop();
				running = false;
				break;
			}
			else if(SDL_KEYDOWN == windowEvent.type)
			{
				if (!inputController.OnKeyJustPressed(windowEvent.key.keysym.sym, mod))
					return;
			}
			else if(SDL_KEYUP == windowEvent.type)
			{
				inputController.OnKeyRelease(windowEvent.key.keysym.sym);
			}
			else if(SDL_MOUSEBUTTONDOWN == windowEvent.type)
			{
				if(windowEvent.motion.x < viewport_offset.x || windowEvent.motion.y < viewport_offset.y || ImGuizmo::IsUsing())
					continue;
				inputController.OnMousePress(windowEvent.motion.x - viewport_offset.x,
																		 windowEvent.motion.y - viewport_offset.y,
																		 windowEvent.button.button == SDL_BUTTON_LEFT,
																		 mod);
			}
			else if(SDL_MOUSEBUTTONUP == windowEvent.type)
			{
				if (ImGuizmo::IsUsing())
					continue;
				inputController.OnMouseRelease(mod);
			}
			else if(SDL_MOUSEMOTION == windowEvent.type)
			{
				if (ImGuizmo::IsUsing())
					continue;
				inputController.OnMouseMove(windowEvent.motion.x - viewport_offset.x,
																		windowEvent.motion.y - viewport_offset.y,
																		mod);
			}
			else if (SDL_MOUSEWHEEL == windowEvent.type)
			{
				inputController.OnMouseWheel(windowEvent.wheel.x, windowEvent.wheel.y, mod);
			}
		}
		if(inputController.IsAnyKeyPressed())
		{
			inputController.OnKeyPress();
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
		flag = true;
	}

#ifndef STANDALONE
	eImGuiContext::GetInstance(&context, window).NewFrame();
	ImGuizmo::BeginFrame();
#endif			

	if(m_disable_system_cursor_under_view)
		SDL_ShowCursor(SDL_DISABLE);
	else
		SDL_ShowCursor(SDL_ENABLE);

#ifndef STANDALONE
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	for (auto* gui : guiWnd)
	{
		if (gui->IsHovered()) //@todo main menu should work as well
		{
			SDL_ShowCursor(SDL_ENABLE);
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		}
	}
#endif

	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	mainContext->PaintGL();

	OnDockSpace();

	/*eWindowImGuiDemo demo;
  demo.Render();*/


#ifndef STANDALONE
	for (auto* gui : guiWnd)
	{
		int window_x, window_y;
		SDL_GetWindowPosition(window, &window_x, &window_y);
		gui->SetWindowOffset(window_x, window_y);
		gui->SetViewportOffset(viewport_offset.x, viewport_offset.y);
		gui->Render();
	}

	eImGuiContext::GetInstance(&context, window).Render();
#endif	
	SDL_GL_SwapWindow(window);
}

//======================================
//dbGLWindowSDL::OnDockSpace
//---------------------------------------
void dbGLWindowSDL::OnDockSpace()
{
#ifndef STANDALONE
	static bool dockspaceOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	
	ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

		ImGui::Begin("Game"); //@todo make gui window with will turn off cursor if hovered and turno on when not othwer then visa vers

		ImVec2 viewport_pos = ImGui::GetWindowPos();
		int window_x, window_y, border_x, border_y;
		SDL_GetWindowPosition(window, &window_x, &window_y);
		SDL_GetWindowBordersSize(window, &border_y, &border_x, nullptr, nullptr);
		border_y -= mysterious_y_border_offset;
		viewport_offset = { viewport_pos.x - window_x + border_x,
												viewport_pos.y - window_y + border_y };
		ImGui::Image((void*)(intptr_t)(mainContext->GetFinalImageId()), ImVec2(WIDTH, HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

		//Guizmo!
		if (auto& obj = mainContext->GetFocusedObject(); mainContext->UseGizmo() && obj)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(viewport_pos.x, viewport_pos.y, WIDTH, HEIGHT);

			glm::mat4 view = mainContext->GetMainCameraViewMatrix();
			glm::mat4 projection= mainContext->GetMainCameraProjectionMatrix();
			glm::mat4 transform = obj->GetTransform()->getModelMatrix();
			ImGuizmo::Manipulate(&view[0][0], &projection[0][0], (ImGuizmo::OPERATION)mainContext->CurGizmoType(), ImGuizmo::LOCAL, &transform[0][0]);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 scale; glm::quat rotation; glm::vec3 translation; glm::vec3 sqew; glm::vec4 perspective;
				glm::decompose(transform, scale, rotation, translation, sqew, perspective);
				obj->GetTransform()->setTranslation(translation);
				obj->GetTransform()->setRotation(rotation);
				obj->GetTransform()->setScale(scale);
			}
		}
		ImGuizmo::SetRect(viewport_pos.x - window_x + border_x, viewport_pos.y - window_y + border_y, WIDTH, HEIGHT);
		//Guizmo end!

		ImGui::End(); //Game

	ImGui::End();
#endif	
}
