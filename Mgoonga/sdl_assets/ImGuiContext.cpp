#include "stdafx.h"
#include "ImGuiContext.h"

#include <SDL2-2.0.9/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL2-2.0.9/include/SDL_opengl.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <algorithm>
#include <functional>

#include <base/base.h>
#include <opengl_assets/Texture.h>

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
	const char* glsl_version = "#version 130"; //330 ?

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiWindowFlags_AlwaysAutoResize;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void eImGuiContext::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
}

void eImGuiContext::Render()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void eImGuiContext::PreRender()
{
	ImGui::Render();
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

void eWindowImGui::Render()
{
  //ImGui::SetNextWindowSize({ 500, 250 });
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
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem(std::get<0>(item).c_str()))
          {
            auto callback = reinterpret_cast<std::function<void()>*>(std::get<2>(item));
            (*callback)();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
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
  ImGui::SetNextWindowPos(ImVec2{ window_pos_x , window_pos_y + window_size_y });
  ImGui::SetNextWindowContentWidth(window_size_x);
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
  bool is_pressed = x > window_pos_x && y > window_pos_y && x < (window_pos_x + window_size_x) && y < (window_pos_y + window_size_y);
  if (is_pressed)
    return true;
  else
    return false;
}

//---------------------------------------------------------------------
bool eWindowImGui::OnMouseMove(uint32_t x, uint32_t y)
{
  bool is_pressed = x > window_pos_x && y > window_pos_y && x < (window_pos_x + window_size_x) && y < (window_pos_y + window_size_y);
  if (is_pressed)
    return true;
  else
    return false;
}
