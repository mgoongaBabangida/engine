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
#include <base/Object.h>
#include <math/Rigger.h>
#include <opengl_assets/Texture.h>

#include <glm/glm/gtc/quaternion.hpp>

imgui_addons::ImGuiFileBrowser file_dialog;

//---------------------------------------------------------------------
struct UniformDisplayVisitor
{
  void operator()(const int32_t& i) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(i).c_str());
  }

  void operator()(const size_t& i) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(i).c_str());
  }

  void operator()(const float& f) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(f).c_str());
  }

  void operator()(const bool& b) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(b).c_str());
  }

  void operator()(const glm::vec2& v) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[0]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[1]).c_str());
  }

  void operator()(const glm::vec3& v) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[0]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[1]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[2]).c_str());
  }

  void operator()(const glm::vec4& v) const {
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[0]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[1]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[2]).c_str());
    ImGui::SameLine();
    ImGui::Text(std::to_string(v[3]).c_str());
  }
  template<class T>
  void operator()(const T&) const {
  }
};

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

  ImVec2 pos = ImGui::GetWindowPos();
  window_pos_x = pos.x;
  window_pos_y = pos.y;
  ImVec2 size = ImGui::GetWindowSize();
  window_size_x = size.x;
  window_size_y = size.y;
  //ImGui::SetNextWindowPos(ImVec2{ window_pos_x , window_pos_y + window_size_y });
  ImGui::SetWindowSize({ 350, 300 });

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
        ImGui::Image((void*)(intptr_t)(std::get<2>(item)), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
      }
      break;
      case SLIDER_FLOAT_3:
      {
        eThreeFloat* transfer_data = static_cast<eThreeFloat*>(std::get<2>(item));
        ImGui::Text(std::get<0>(item).c_str());
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGui::SliderFloat3(std::get<0>(item).c_str(), &transfer_data->data[0],-10, 10); //@todo
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
          ImGui::Image((void*)(intptr_t)(t->id), ImVec2(240, 160)/* ImVec2(t->mTextureWidth, t->mTextureHeight)*/, ImVec2(0, 1), ImVec2(1, 0));
        }
      }
      break;
      case BUTTON:
      {
        if (ImGui::Button(std::get<0>(item).c_str()))
        {
          if(auto callback = callbacks.find(std::get<0>(item)); callback!= callbacks.end())
            callback->second();
        }
      }
      break;
      case OBJECT_REF:
      {
        //check if obj has changed and put current mesh to 0 (save last obj?)
        shObject* p_object = static_cast<shObject*>(std::get<2>(item));
        if (p_object && *p_object)
        {
          shObject obj = *p_object;
          ImGui::Text(obj->Name().c_str());

          static glm::vec3 g_translation, g_rotation, g_scale;
          ImGui::Text("");
          g_translation[0] = obj->GetTransform()->getTranslation()[0];
          g_translation[1] = obj->GetTransform()->getTranslation()[1];
          g_translation[2] = obj->GetTransform()->getTranslation()[2];
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("Position", &g_translation[0], -10, 10))
          {
            obj->GetTransform()->setTranslation(g_translation);
          }

          ImGui::Text("Rotation");
          
         /* auto euler = glm::eulerAngles(obj->GetTransform()->getRotation());
          std::cout << glm::abs(static_cast<float>(euler.z) - static_cast<float>(g_rotation.z)) << std::endl;
          */
          
          glm::vec3 rot = g_rotation;
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("", &g_rotation[0], -PI * 2, PI * 2, "%.2f"))
          {
            auto euler = glm::eulerAngles(obj->GetTransform()->getRotation());
            //std::cout << "euler " << euler.x << " " << euler.y<< " " << euler.z << std::endl;
            //auto xRotation = glm::cross(glm::vec3(obj->GetTransform()->getRotationUpVector()), glm::vec3(obj->GetTransform()->getRotationVector()));
            auto xRotation = glm::vec3(1.0f, 0.0f, 0.0f);
            float angleX = static_cast<float>(g_rotation[0]) - static_cast<float>(rot[0]);
            glm::quat rotX = glm::angleAxis(angleX, xRotation);
            
            auto yRotation = glm::vec3(0.0f, 1.0f, 0.0f); //glm::vec3(obj->GetTransform()->getRotationUpVector())
            float angleY = static_cast<float>(g_rotation[1]) - static_cast<float>(rot[1]);
            glm::quat rotY = glm::angleAxis(angleY, yRotation);
            
            auto zRotation = glm::vec3(0.0f, 0.0f, 1.0f); //lm::vec3(obj->GetTransform()->getRotationVector())
            float angleZ = static_cast<float>(g_rotation[2]) - static_cast<float>(rot[2]);
            glm::quat rotZ = glm::angleAxis(angleZ, zRotation);

            obj->GetTransform()->setRotation(rotX * rotY * rotZ* obj->GetTransform()->getRotation());
          }

          ImGui::Text("");
          g_scale[0] = obj->GetTransform()->getScaleAsVector().x;
          g_scale[1] = obj->GetTransform()->getScaleAsVector().y;
          g_scale[2] = obj->GetTransform()->getScaleAsVector().z;
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("Scale", &g_scale[0], 0.0f, 5.0f))
          {
            obj->GetTransform()->setScale(g_scale);
          }
          std::string text = std::string("Number of vertices ") + std::to_string(obj->GetModel()->GetVertexCount());
          ImGui::Text(text.c_str());
          
          text = std::string("Number of meshes ") + std::to_string(obj->GetModel()->GetMeshCount());
          ImGui::Text(text.c_str());

          //Meshes transfer to rigger later
          combo_list.clear();
          for (size_t i = 0; i < obj->GetModel()->GetMeshCount(); ++i)
          {
            std::string name = obj->GetModel()->GetMeshes()[i]->Name();
            for (int j = 0; j < name.size(); ++j)
              combo_list.push_back(name[j]);
            if(i != obj->GetModel()->GetMeshCount() -1)
              combo_list.push_back('\0');
          }
          
          static int mesh_current = 0;
          if (!combo_list.empty())
          {
            if (ImGui::Combo("Object Meshes", &mesh_current, &combo_list[0]))
            {
            }
          }

          // Material
          Material material;
          if (obj->GetModel()->GetMeshes()[mesh_current]->HasMaterial())
            material = *(obj->GetModel()->GetMeshes()[mesh_current]->GetMaterial());
          else if (obj->GetModel()->HasMaterial())
            material = *(obj->GetModel()->GetMaterial());
          else
            assert(false && "Neither mesh nor model have material!");

          ImGui::Text("Albedo texture(Diffuse)");
          ImGui::Image((void*)(intptr_t)(material.albedo_texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.albedo[0]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.albedo[1]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.albedo[2]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.use_albedo).c_str());

          ImGui::Text("Metalic texture");
          ImGui::Image((void*)(intptr_t)(material.metalic_texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.metallic).c_str());
          ImGui::Text(std::to_string(material.use_metalic).c_str());

          ImGui::Text("AO"); ImGui::SameLine();
          ImGui::Text(std::to_string(material.ao).c_str());

          ImGui::Text("Normal texture(Bump)");
          ImGui::Image((void*)(intptr_t)(material.normal_texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.use_normal).c_str());

          ImGui::Text("Roughness texture");
          ImGui::Image((void*)(intptr_t)(material.roughness_texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.roughness).c_str());
          ImGui::Text(std::to_string(material.use_roughness).c_str());

          ImGui::Text("Emissive texture");
          ImGui::Image((void*)(intptr_t)(material.emissive_texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));

          combo_list.clear();

          if (obj->GetRigger())
          {
            //Animation -> transfer to rigger as well
            text = std::string("Number of animations ") + std::to_string(obj->GetRigger()->GetAnimationCount());
            ImGui::Text(text.c_str());

            //Animations
            Rigger* rigger = dynamic_cast<Rigger*>(obj->GetRigger());
            std::vector<std::string> names = rigger->GetAnimationNames();
            for (size_t i = 0; i < obj->GetRigger()->GetAnimationCount(); ++i)
            {
              for (int j = 0; j < names[i].size(); ++j)
                combo_list.push_back(names[i][j]);
              if (i != names[i].size() - 1)
                combo_list.push_back('\0');
            }

            static int anim_current = 0;
            if (!combo_list.empty())
            {
              if (ImGui::Combo("Object Animations", &anim_current, &combo_list[0]))
              {
              }
            }
            combo_list.clear();

            //Frames
            if (auto* cur_anim = rigger->GetCurrentAnimation(); cur_anim != nullptr)
            {
              text = std::string("Number of frames ") + std::to_string(rigger->GetCurrentAnimation()->GetNumFrames());
              ImGui::Text(text.c_str());

              for (size_t i = 0; i < cur_anim->GetNumFrames(); ++i)
              {
                std::string number = std::to_string(i);
                for (int j = 0; j < number.size(); ++j)
                  combo_list.push_back(number[j]);
                if (i != cur_anim->GetNumFrames() - 1)
                  combo_list.push_back('\0');
              }

              static int frame_current = 0;
              if (!combo_list.empty())
              {
                if (ImGui::Combo("Object Frames", &frame_current, &combo_list[0]))
                {
                }
              }

              static bool is_frame_freez = false;
              ImGui::Checkbox("FreezeFrame frame", &is_frame_freez);
              if (is_frame_freez)
                rigger->GetCurrentAnimation()->FreezeFrame(frame_current);
              else
                rigger->GetCurrentAnimation()->FreezeFrame(-1);
            }

            combo_list.clear();
            //Bones
            text = std::string("Number of bones ") + std::to_string(obj->GetRigger()->GetBoneCount());
            ImGui::Text(text.c_str());
            std::vector<std::string> bone_names = rigger->GetBoneNames();
            for (size_t i = 0; i < obj->GetRigger()->GetBoneCount(); ++i)
            {
              for (int j = 0; j < bone_names[i].size(); ++j)
                combo_list.push_back(bone_names[i][j]);
              if (i != bone_names[i].size() - 1)
                combo_list.push_back('\0');
            }
            static int bone_current = 0;
            static glm::mat4 boneMatrix;
            if (!combo_list.empty())
            {
              if (ImGui::Combo("Object Bones", &bone_current, &combo_list[0]))
              {
                boneMatrix = rigger->GetCurrentMatrixForBone(bone_current);
              }
            }
            combo_list.clear();

            ImGui::Text("Bone animated transform Matrix");
            ImGui::Text(std::to_string(boneMatrix[0][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[0][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[0][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[0][3]).c_str());

            ImGui::Text(std::to_string(boneMatrix[1][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[1][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[1][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[1][3]).c_str());

            ImGui::Text(std::to_string(boneMatrix[2][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[2][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[2][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[2][3]).c_str());

            ImGui::Text(std::to_string(boneMatrix[3][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[3][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[3][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(boneMatrix[3][3]).c_str());

            static glm::mat4 bindMatrix = rigger->GetBindMatrixForBone(bone_current);

            ImGui::Text("Bone bind transform Matrix");
            ImGui::Text(std::to_string(bindMatrix[0][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[0][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[0][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[0][3]).c_str());

            ImGui::Text(std::to_string(bindMatrix[1][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[1][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[1][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[1][3]).c_str());

            ImGui::Text(std::to_string(bindMatrix[2][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[2][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[2][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[2][3]).c_str());

            ImGui::Text(std::to_string(bindMatrix[3][0]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[3][1]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[3][2]).c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(bindMatrix[3][3]).c_str());

            //Play animation
            if (ImGui::Button("Play current animations "))
            {
              if (obj->GetModel()->GetAnimationCount() != 0)
                obj->GetRigger()->Apply(anim_current, false);
            }
            if (ImGui::Button("Play once current animations "))
            {
              if (obj->GetModel()->GetAnimationCount() != 0)
                obj->GetRigger()->Apply(anim_current, true);
            }
            if (ImGui::Button("Stop current animations "))
            {
              obj->GetRigger()->Stop();
            }
          }
        }
      }
      break;
      case SHADER:
      {
        const std::vector<ShaderInfo>* infos = static_cast<const std::vector<ShaderInfo>*>(std::get<2>(item));

        combo_list.clear();
        for (size_t i = 0; i < infos->size(); ++i)
        {
          std::string name = (*infos)[i].name;
          for (int j = 0; j < name.size(); ++j)
            combo_list.push_back(name[j]);
          if (i != infos->size() - 1)
            combo_list.push_back('\0');
        }

        static int shader_current = 0;
        if (!combo_list.empty())
        {
          if (ImGui::Combo("Shaders", &shader_current, &combo_list[0]))
          {
          }
          //if (ImGui::Button("Update shaders"))
          //{
          //  shader_current = 0;
          //  if (auto callback = callbacks.find("Update shaders"); callback != callbacks.end())
          //    callback->second();
          //}
          for (const auto& uniform : (*infos)[shader_current].uniforms)
          {
            ImGui::Text(uniform.name.c_str());
            std::visit(UniformDisplayVisitor(), uniform.data);
          }
        }

      }
      break;
    }
  }

	ImGui::End();
}

//-----------------------------------------------------------------------
void eWindowImGui::Add(TypeImGui _type, const std::string & _name, void* _data)
{
  if (_type == BUTTON)
  {
    auto callback = reinterpret_cast<std::function<void()>*>(_data);
    callbacks.insert({_name, *callback });
  }

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

//-----------------------------------------------------------------
void eMainImGuiWindow::Render()
{
  bool open = false, save = false, open_file = false;
  ImGui::BeginMainMenuBar();
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Open", NULL))
      open = true;
    if (ImGui::MenuItem("Save", NULL))
      save = true;

    for (auto& item : lines)
    {
      switch (std::get<1>(item))
      {
      case MENU:
      {
        if (ImGui::MenuItem(std::get<0>(item).c_str()))
        {
          auto callback = reinterpret_cast<std::function<void()>*>(std::get<2>(item));
          (*callback)();
        }
      }
      break;
      case MENU_OPEN:
      {
        if (ImGui::MenuItem(std::get<0>(item).c_str(), NULL))
        {
          open_file = true;
          open_file_menu_name = std::get<0>(item);
          open_file_callback = *(reinterpret_cast<std::function<void(const std::string&)>*>(std::get<2>(item)));
        }
      }
      break;
      }
    }
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  if (open)
    ImGui::OpenPopup("Open File");
  if (save)
    ImGui::OpenPopup("Save File");
  if(open_file)
    ImGui::OpenPopup(open_file_menu_name.c_str());

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
  if (file_dialog.showFileDialog(open_file_menu_name.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".obj"))
  {
    open_file_callback(file_dialog.selected_path);
  }
}

//-------------------------------------------------------------------
void eMainImGuiWindow::Add(TypeImGui _type, const std::string& _name, void* _data)
{
  auto it = std::find_if(lines.begin(), lines.end(), [&_name](const eItem& _item)
    { return std::get<0>(_item) == _name; });
  if (it == lines.end())
    lines.push_back(eItem(_name, _type, _data));
  else
    std::get<2>(*it) = _data;
}
