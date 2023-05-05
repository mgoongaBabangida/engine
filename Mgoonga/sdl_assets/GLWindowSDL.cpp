#include "stdafx.h"
#include "GLWindowSDL.h"

#include "ImGuiContext.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"

#include <base/interfaces.h>
#include <base/Object.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include "ImGuizmo.h"

#include <glm/glm/gtx/matrix_decompose.hpp>

SDL_GLContext					context;
ImVec2								viewport_offset;


//***************************************
//dbGLWindowSDL::~dbGLWindowSDL
//---------------------------------------
dbGLWindowSDL::dbGLWindowSDL(const IGameFactory& _factory)
: inputController()
{
  guiWnd.push_back(new eWindowImGui("Lights & Cameras"));//0
  guiWnd.push_back(new eWindowImGui("Pipeline"));//1
  guiWnd.push_back(new eWindowImGui("Object Transform"));//2
	guiWnd.push_back(new eWindowImGui("Shader"));//3
	guiWnd.push_back(new eMainImGuiWindow());//4
	guiWnd.push_back(new eWindowImGui("Create"));//5
	guiWnd.push_back(new eWindowImGui("Objects list"));//6
	guiWnd.push_back(new eWindowImGui("Object Material"));//7
	guiWnd.push_back(new eWindowImGui("Object Rigger"));//8

  on_close = std::function<void()>{ [this](){this->Close(); } };
  guiWnd[4]->Add(MENU, "Close", reinterpret_cast<void*>(&on_close));

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
		return false;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	
	window	= SDL_CreateWindow("OpenGl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH+575, HEIGHT+50, //@todo
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_ALLOW_HIGHDPI);
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
			/*if (io.WantCaptureMouse)
				continue;*/
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
				if(windowEvent.motion.x < viewport_offset.x || windowEvent.motion.y < viewport_offset.y)
					continue;
				inputController.OnMousePress(windowEvent.motion.x - viewport_offset.x,
																		 windowEvent.motion.y - viewport_offset.y,
																		 windowEvent.button.button == SDL_BUTTON_LEFT);
			}
			else if(SDL_MOUSEBUTTONUP == windowEvent.type)
			{
				inputController.OnMouseRelease();
			}
			else if(SDL_MOUSEMOTION == windowEvent.type)
			{
				if (windowEvent.motion.x < viewport_offset.x || windowEvent.motion.y < viewport_offset.y)
					continue;
				inputController.OnMouseMove(windowEvent.motion.x - viewport_offset.x,
																		windowEvent.motion.y - viewport_offset.y);
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
		flag = true;
	}
	eImGuiContext::GetInstance(&context, window).NewFrame();

	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	mainContext->PaintGL();

	OnDockSpace();

	/*eWindowImGuiDemo demo;
  demo.Render();*/

	for (auto* gui : guiWnd)
	{
		int window_x, window_y;
		SDL_GetWindowPosition(window, &window_x, &window_y);
		gui->SetWindowOffset(window_x, window_y);
		gui->SetViewportOffset(viewport_offset.x, viewport_offset.y);
		gui->Render();
	}
	eImGuiContext::GetInstance(&context, window).Render();
	SDL_GL_SwapWindow(window);
}

//======================================
//dbGLWindowSDL::OnDockSpace
//---------------------------------------
void dbGLWindowSDL::OnDockSpace()
{
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

	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("Options"))
	//	{
	//		// Disabling fullscreen would allow the window to be moved to the front of other windows,
	//		// which we can't undo at the moment without finer window depth/z control.
	//		ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
	//		ImGui::MenuItem("Padding", NULL, &opt_padding);
	//		ImGui::Separator();

	//		if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
	//		if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
	//		if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
	//		if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
	//		if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
	//		ImGui::Separator();

	//		if (ImGui::MenuItem("Close", NULL, false, dockspaceOpen != NULL))
	//			dockspaceOpen = false;
	//		ImGui::EndMenu();
	//	}

	//	ImGui::EndMenuBar();
	//}

		ImGui::Begin("Game");

		ImVec2 viewport_pos = ImGui::GetWindowPos();
		int window_x, window_y, border_x, border_y;
		SDL_GetWindowPosition(window, &window_x, &window_y);
		SDL_GetWindowBordersSize(window, &border_y, &border_x, nullptr, nullptr);
		border_y -= 5;
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
			}
		}
		ImGuizmo::SetRect(viewport_pos.x - window_x + border_x, viewport_pos.y - window_y + border_y, WIDTH, HEIGHT);
		ImGui::End();

	ImGui::End();
}
