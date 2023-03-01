#include "stdafx.h"
#include "ImGuiContext.h"

#include <SDL/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL/include/SDL_opengl.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ImGuiFileBrowser.h"

#include <algorithm>
#include <functional>
#include <iostream>

#include <base/base.h>
#include <opengl_assets/Texture.h>

imgui_addons::ImGuiFileBrowser file_dialog;

eImGuiContext::eImGuiContext(SDL_GLContext* _context, SDL_Window* _window)
	:context(_context), window(_window)
{}

eImGuiContext& eImGuiContext::GetInstance(SDL_GLContext* context, SDL_Window* window)
{
	static eImGuiContext  instance(context, window);
	return instance;
}

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

void eImGuiContext::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(); // pass window or not?
	ImGui::NewFrame();
}

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
}

void eImGuiContext::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

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

eWindowImGui::eWindowImGui(const std::string & _name)
  :name(_name)
{
}

void eWindowImGui::SetViewportOffset(float x_offset, float y_offset)
{
  viewport_offset_x = x_offset;
  viewport_offset_y = y_offset;
}

void eWindowImGui::SetWindowOffset(float x_offset, float y_offset)
{
  window_offset_x = x_offset;
  window_offset_y = y_offset;
}

void eWindowImGui::Render()
{
  ImGui::Begin(name.c_str(), &visible);

  for (auto& item : lines)
  {
    switch (std::get<1>(item))
    {
      case SLIDER_FLOAT: ImGui::SliderFloat(std::get<0>(item).c_str(), static_cast<float*>(std::get<2>(item)), -10.0f, 10.0f); break; // -10 10 $todo
      case TEXT:         ImGui::Text(std::get<0>(item).c_str()); break;
      case CHECKBOX:     ImGui::Checkbox(std::get<0>(item).c_str(), static_cast<bool*>(std::get<2>(item))); break;
      case TEXTURE:
      {
        ImGui::Text(std::get<0>(item).c_str());
        ImGui::Image((void*)(intptr_t)(std::get<2>(item)), ImVec2(240, 160), ImVec2(1, 1), ImVec2(0, 0));
      }
      break;
      case MENU:
      {
        bool open = false, save = false;
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Open", NULL))
            open = true;
          if (ImGui::MenuItem("Save", NULL))
            save = true;
          if (ImGui::MenuItem(std::get<0>(item).c_str()))
          {
            auto callback = reinterpret_cast<std::function<void()>*>(std::get<2>(item));
            (*callback)();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        if (open)
          ImGui::OpenPopup("Open File");
        if (save)
          ImGui::OpenPopup("Save File");

        /* Optional third parameter. Support opening only compressed rar/zip files.
     * Opening any other file will show error, return false and won't close the dialog.
     */
        if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".rar,.zip,.7z,.obj"))
        {
          std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
          std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
        }
        if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
          {
          std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
          std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
          std::cout << file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)
          //Do writing of files based on extension here
          }
      }
      break;
      case SLIDER_FLOAT_3_CALLBACK:
      {
        eThreeFloatCallback* transfer_data = static_cast<eThreeFloatCallback*>(std::get<2>(item));
        ImGui::Text(std::get<0>(item).c_str());
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        if (ImGui::SliderFloat3(std::get<0>(item).c_str(), &transfer_data->data[0], transfer_data->min, transfer_data->max))
          transfer_data->callback();
      }
      break;
      case TEXT_INT:
      {
        size_t* data = static_cast<size_t*>(std::get<2>(item));
        std::string text = std::get<0>(item) + std::to_string(*data);
        ImGui::Text(text.c_str()); break;
      }
      break;
      case COMBO_BOX:
      {
        eVectorStringsCallback* transfer_data = static_cast<eVectorStringsCallback*>(std::get<2>(item));
        std::vector<char*> cstrings;
        cstrings.reserve(transfer_data->data.size());

        for (size_t i = 0; i < transfer_data->data.size(); ++i)
          cstrings.push_back(const_cast<char*>(transfer_data->data[i].c_str()));

        static int item_current = 0;
        if (!cstrings.empty())
        {
          if (ImGui::Combo(std::get<0>(item).c_str(), &item_current, &cstrings[0], transfer_data->data.size()))
            transfer_data->callback(item_current);
        }
      }
      break;
      case TEXTURE_ARRAY:
      {
        std::vector<const Texture*>* transfer_data = static_cast<std::vector<const Texture*>*>(std::get<2>(item));
        ImGui::Text(std::get<0>(item).c_str());
        for (const Texture* t : *transfer_data)
        {
          ImGui::Image((void*)(intptr_t)(t->id), ImVec2(240, 160)/* ImVec2(t->mTextureWidth, t->mTextureHeight)*/, ImVec2(1, 1), ImVec2(0, 0));
        }
      }
      break;
      case BUTTON:
      {
        if (ImGui::Button(std::get<0>(item).c_str()))
        {
          std::function<void()> callback = *static_cast<std::function<void()>*>(std::get<2>(item));
          callback();
        }
      }
      break;
    }
  }

  ImVec2 pos = ImGui::GetWindowPos();
  window_pos_x = pos.x;
  window_pos_y = pos.y;
  ImVec2 size = ImGui::GetWindowSize();
  window_size_x =size.x;
  window_size_y =size.y;
  //ImGui::SetNextWindowPos(ImVec2{ window_pos_x , window_pos_y + window_size_y });
  ImGui::SetWindowSize({ 300, 300 });
	ImGui::End();
}

void eWindowImGui::Add(TypeImGui _type, const std::string & _name, void* _data)
{
	auto it = std::find_if(lines.begin(), lines.end(), [&_name](const eItem& _item)
													   { return std::get<0>(_item) == _name; });
	if (it == lines.end())
		lines.push_back(eItem(_name, _type, _data));
	else
		std::get<2>(*it) = _data;
}

//---------------------------------------------------------------------
bool eWindowImGui::OnMousePress(uint32_t x, uint32_t y, bool left)
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
bool eWindowImGui::OnMouseMove(uint32_t x, uint32_t y)
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

