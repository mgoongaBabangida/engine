#include "stdafx.h"
#include "ImGuiContext.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

static bool	block_events = false;
bool eImGuiContext::is_initialized = false;

//---------------------------------------------------------------------
eImGuiContext::eImGuiContext(SDL_GLContext* _context, SDL_Window* _window)
	:context(_context), window(_window)
{}

//---------------------------------------------------------------------
eImGuiContext& eImGuiContext::GetInstance(SDL_GLContext* context, SDL_Window* window)
{
	static eImGuiContext  instance(context, window);
	return instance;
}

//---------------------------------------------------------------------
void eImGuiContext::Init()
{
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 330";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;     // Disable SDL Cursor
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

  /*ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }*/

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

//---------------------------------------------------------------------
void eImGuiContext::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(); // pass window or not?
	ImGui::NewFrame();
}

//---------------------------------------------------------------------
void eImGuiContext::Render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
  }
  is_initialized = true;
}

//---------------------------------------------------------------------
void eImGuiContext::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

//---------------------------------------------------------------------
bool eImGuiContext::IsInitialized()
{
  return is_initialized;
}

//---------------------------------------------------------------------
bool eImGuiContext::BlockEvents()
{
  return block_events;
}

//---------------------------------------------------------------------
void eImGuiContext::SetBlockEvents(bool _b)
{
  block_events = _b;
}

//---------------------------------------------------------------------
void eWindowImGuiDemo::Render()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool show_demo_window = true;
	bool show_another_window = false;

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

//---------------------------------------------------------------------
void eWindowImGui::SetViewportOffset(float x_offset, float y_offset)
{
  viewport_offset_x = x_offset;
  viewport_offset_y = y_offset;
}

//---------------------------------------------------------------------
void eWindowImGui::SetWindowOffset(float x_offset, float y_offset)
{
  window_offset_x = x_offset;
  window_offset_y = y_offset;
}

//---------------------------------------------------------------------
bool eWindowImGui::OnMousePress(int32_t x, int32_t y, bool left, KeyModifiers _modifier)
{
  float global_pos_x = x + viewport_offset_x + window_offset_x;
  float global_pos_y = y + viewport_offset_y + window_offset_y;

  bool is_pressed = global_pos_x > window_pos_x &&
                    global_pos_y > window_pos_y &&
                    global_pos_x < (window_pos_x + window_size_x) &&
                    global_pos_y < (window_pos_y + window_size_y);

  if (is_pressed)
    return true;
  else
    return false;
}

//---------------------------------------------------------------------
bool eWindowImGui::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
  cursor_x = (float)_x;
  cursor_y = (float)_y;
  if (IsHovered())
    return true;
  else
    return false;
}

//-----------------------------------------------------------------
bool eWindowImGui::OnMouseWheel(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
  if (IsHovered())
    return true;
  else
    return false;
}

//-----------------------------------------------------------------
bool eWindowImGui::IsHovered()
{
  float global_pos_x = cursor_x + viewport_offset_x + window_offset_x;
  float global_pos_y = cursor_y + viewport_offset_y + window_offset_y;
  return global_pos_x > window_pos_x &&
         global_pos_y > window_pos_y &&
         global_pos_x < (window_pos_x + window_size_x) &&
         global_pos_y < (window_pos_y + window_size_y);
}
