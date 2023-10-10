#include "stdafx.h"
#include "ImGuiContext.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include "ImGuiFileBrowser.h"

#include <algorithm>
#include <functional>
#include <iostream>

#include <base/base.h>
#include <math/Rigger.h>
#include <opengl_assets/Texture.h>

#include <glm/glm/gtc/quaternion.hpp>

imgui_addons::ImGuiFileBrowser file_dialog;

static bool	block_events = false;

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

bool eImGuiContext::BlockEvents()
{
  return block_events;
}

void eImGuiContext::SetBlockEvents(bool _b)
{
  block_events = _b;
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
  : name(_name)
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
  ImGui::SetWindowSize({ 400, 300 }); //@todo why this size?

  for (auto& item : lines)
  {
    switch (std::get<1>(item))
    {
      case SLIDER_FLOAT:
      {
        float* f = static_cast<float*>(std::get<2>(item));
        ImGui::SliderFloat(std::get<0>(item).c_str(), f, -10.0f, 10.0f); // -10 10 @todo

        ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
        if (ImGui::IsItemHovered())
        {
          float wheel = ImGui::GetIO().MouseWheel;
          if (wheel)
          {
            if (ImGui::IsItemActive())
            {
              ImGui::ClearActiveID();
            }
            else
            {
              *f += wheel;
            }
          }
        }

      } break;

      case SLIDER_FLOAT_LARGE:  ImGui::SliderFloat(std::get<0>(item).c_str(), static_cast<float*>(std::get<2>(item)), -250.0f, 250.0f); break;
      case SLIDER_INT:          ImGui::SliderInt(std::get<0>(item).c_str(), static_cast<int*>(std::get<2>(item)), 0, 1024); break; //@todo
      case SLIDER_INT_NERROW:   ImGui::SliderInt(std::get<0>(item).c_str(), static_cast<int*>(std::get<2>(item)), 0, 20); break;
      case TEXT:                ImGui::Text(std::get<0>(item).c_str()); break;
      case CHECKBOX:            ImGui::Checkbox(std::get<0>(item).c_str(), static_cast<bool*>(std::get<2>(item))); break;
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
      case SPIN_BOX:
      {
        static int value = 0; //@todo should be unique for each spinbox
        if (ImGui::InputInt(std::get<0>(item).c_str(), &value))
        {
          std::function<void(int)> callback = *(reinterpret_cast<std::function<void(int)>*>(std::get<2>(item)));
          callback(value);
        }
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
      case OBJECT_REF_TRANSFORM:
      {
        shObject* p_object = static_cast<shObject*>(std::get<2>(item));
        if (p_object && *p_object)
        {
          if (*p_object != m_current_object)
          {
            m_current_object = *p_object;
            m_combo_list.clear();
          }
          ImGui::Text(m_current_object->Name().c_str());

          static glm::vec3 g_translation, g_rotation, g_scale;
          ImGui::Text("");
          g_translation[0] = m_current_object->GetTransform()->getTranslation()[0];
          g_translation[1] = m_current_object->GetTransform()->getTranslation()[1];
          g_translation[2] = m_current_object->GetTransform()->getTranslation()[2];
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("Position", &g_translation[0], -10, 10))
          {
            m_current_object->GetTransform()->setTranslation(g_translation);
          }

          ImGui::Text("Rotation");
          
          //prob assign g_rotation only when changing the object
          glm::vec3 rot = g_rotation = glm::eulerAngles(m_current_object->GetTransform()->getRotation());
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("", &g_rotation[0], -PI * 2, PI * 2, "%.2f"))
          {
            auto euler = glm::eulerAngles(m_current_object->GetTransform()->getRotation());
            //std::cout << "euler " << euler.x << " " << euler.y<< " " << euler.z << std::endl;
            auto xRotation = glm::cross(glm::vec3(m_current_object->GetTransform()->getRotationUpVector()),
              glm::vec3(m_current_object->GetTransform()->getRotationVector()));
            float angleX = static_cast<float>(g_rotation[0]) - static_cast<float>(rot[0]);
            glm::quat rotX = glm::angleAxis(angleX, xRotation);
            
            auto yRotation = glm::vec3(m_current_object->GetTransform()->getRotationUpVector());
            float angleY = static_cast<float>(g_rotation[1]) - static_cast<float>(rot[1]);
            glm::quat rotY = glm::angleAxis(angleY, yRotation);
            
            auto zRotation = glm::vec3(m_current_object->GetTransform()->getRotationVector());
            float angleZ = static_cast<float>(g_rotation[2]) - static_cast<float>(rot[2]);
            glm::quat rotZ = glm::angleAxis(angleZ, zRotation);

            m_current_object->GetTransform()->setRotation(rotX * rotY * rotZ * m_current_object->GetTransform()->getRotation());
          }

          ImGui::Text("");
          g_scale[0] = m_current_object->GetTransform()->getScaleAsVector().x;
          g_scale[1] = m_current_object->GetTransform()->getScaleAsVector().y;
          g_scale[2] = m_current_object->GetTransform()->getScaleAsVector().z;
          ImGui::PushItemWidth(ImGui::GetWindowWidth());
          if (ImGui::SliderFloat3("Scale", &g_scale[0], 0.0f, 5.0f))
          {
            m_current_object->GetTransform()->setScale(g_scale);
          }
          std::string text = std::string("Number of vertices ") + std::to_string(m_current_object->GetModel()->GetVertexCount());
          ImGui::Text(text.c_str());
          
          text = std::string("Number of meshes ") + std::to_string(m_current_object->GetModel()->GetMeshCount());
          ImGui::Text(text.c_str());
        }
      }
      break;
      case OBJECT_REF_MATERIAL:
      {
        shObject* p_object = static_cast<shObject*>(std::get<2>(item));
        if (p_object && *p_object)
        {
          shObject obj = *p_object;
          ImGui::Text(obj->Name().c_str());

          //Meshes transfer to rigger later
          m_combo_list.clear();
          for (size_t i = 0; i < obj->GetModel()->GetMeshCount(); ++i)
          {
            std::string name = obj->GetModel()->GetMeshes()[i]->Name();
            for (int j = 0; j < name.size(); ++j)
              m_combo_list.push_back(name[j]);
            if (i != obj->GetModel()->GetMeshCount() - 1)
              m_combo_list.push_back('\0');
          }

          static int mesh_current = 0;
          if (!m_combo_list.empty())
          {
            if (ImGui::Combo("Object Meshes", &mesh_current, &m_combo_list[0]))
            {
            }
          }

          // Material
          Material material;
          if (obj->GetModel()->GetMeshes()[mesh_current]->HasMaterial())
            material = *(obj->GetModel()->GetMeshes()[mesh_current]->GetMaterial());
          else if (obj->GetModel()->HasMaterial())
            material = *(obj->GetModel()->GetMaterial());

          //@todo terrain has to have material
          /*else
            assert(false && "Neither mesh nor model have material!");*/

          GLuint texture_id;

          ImGui::Text("Albedo texture (Diffuse)");
          texture_id = material.albedo_texture_id != Texture::GetDefaultTextureId() ? material.albedo_texture_id : Texture::GetEmptyTextureId();
          ImGui::Image((void*)(intptr_t)(texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));

          ImGui::Text(std::to_string(material.albedo[0]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.albedo[1]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.albedo[2]).c_str()); ImGui::SameLine();
          ImGui::Text(std::to_string(material.use_albedo).c_str());

          ImGui::Text("Metalic texture");
          texture_id = material.metalic_texture_id != Texture::GetDefaultTextureId() ? material.metalic_texture_id : Texture::GetEmptyTextureId();
          ImGui::Image((void*)(intptr_t)(texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.metallic).c_str());
          ImGui::Text(std::to_string(material.use_metalic).c_str());

          ImGui::Text("AO"); ImGui::SameLine();
          ImGui::Text(std::to_string(material.ao).c_str());

          ImGui::Text("Normal texture");
          texture_id = material.normal_texture_id != Texture::GetDefaultTextureId() ? material.normal_texture_id : Texture::GetEmptyTextureId();
          ImGui::Image((void*)(intptr_t)(texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.use_normal).c_str());

          ImGui::Text("Roughness texture");//contra glossiness
          texture_id = material.roughness_texture_id != Texture::GetDefaultTextureId() ? material.roughness_texture_id : Texture::GetEmptyTextureId();
          ImGui::Image((void*)(intptr_t)(texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
          ImGui::Text(std::to_string(material.roughness).c_str());
          ImGui::Text(std::to_string(material.use_roughness).c_str());

          ImGui::Text("Emissive texture");
          texture_id = material.emissive_texture_id != Texture::GetDefaultTextureId() ? material.emissive_texture_id : Texture::GetEmptyTextureId();
          ImGui::Image((void*)(intptr_t)(texture_id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));

          //@todo add displacement/ bump as separate slot

          m_combo_list.clear();
        }
      }
      break;
      case OBJECT_REF_RIGGER:
      {
        static const char* current_mesh_item = NULL;
        static const char* current_animation_item = NULL;
        static const char* current_frame_item = NULL;
        static const char* current_bone_item = NULL;
        static const char* current_bone_child_item = NULL;
        static const char* current_game_object_item = NULL;
        static Rigger* g_rigger = nullptr;

        shObject* p_object = static_cast<shObject*>(std::get<2>(item));
        if (p_object && *p_object)
        {
          if (*p_object != m_current_object || (*p_object)->GetRigger() != g_rigger)
          {
            m_current_object = *p_object;

            mesh_names.clear();
            current_mesh_item = m_current_object->GetModel()->GetMeshes()[0]->Name().c_str();
            for (size_t i = 0; i < m_current_object->GetModel()->GetMeshCount(); ++i)
              mesh_names.push_back(m_current_object->GetModel()->GetMeshes()[i]->Name() + " " + std::to_string(i));

            animation_names.clear();
            frame_names.clear();
            bone_names.clear();
            if (m_current_object->GetRigger())
            {
              Rigger* rigger = dynamic_cast<Rigger*>(m_current_object->GetRigger());
              animation_names = rigger->GetAnimationNames();
              if (!animation_names.empty())
              {
                current_animation_item = animation_names[0].c_str();
                rigger->SetCurrentAnimation(animation_names[0]);
              }
              for (size_t i = 0; i < rigger->GetCurrentAnimation()->GetNumFrames(); ++i)
                frame_names.push_back(std::to_string(i));

              current_frame_item = frame_names[0].c_str();
              bone_names = rigger->GetBoneNames();
              current_bone_item = bone_names[0].c_str();
              g_rigger = rigger;
            }
          }

          if (!p_object)
            return;

          // Name
          shObject obj = *p_object;
          ImGui::Text(obj->Name().c_str());

          //Meshes
          if (ImGui::BeginCombo("Object Meshes", current_mesh_item)) // The second parameter is the label previewed before opening the combo.
          {
            for (int n = 0; n < mesh_names.size(); n++)
            {
              bool is_selected = (current_mesh_item == mesh_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
              if (ImGui::Selectable(mesh_names[n].c_str(), is_selected))
                current_mesh_item = mesh_names[n].c_str();
              if (is_selected)
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
            ImGui::EndCombo();
          }

          //Rigger
          if (obj->GetRigger())
          {
            Rigger* rigger = dynamic_cast<Rigger*>(m_current_object->GetRigger());

            std::string text = std::string("Number of animations ") + std::to_string(obj->GetRigger()->GetAnimationCount());
            ImGui::Text(text.c_str());

            //Animations
            if (ImGui::BeginCombo("Object Animations", current_animation_item)) // The second parameter is the label previewed before opening the combo.
            {
              for (int n = 0; n < animation_names.size(); n++)
              {
                bool is_selected = (current_animation_item == animation_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(animation_names[n].c_str(), is_selected))
                {
                  frame_names.clear();
                  current_animation_item = animation_names[n].c_str();
                  rigger->SetCurrentAnimation(animation_names[n]);
                  for (size_t i = 0; i < rigger->GetCurrentAnimation()->GetNumFrames(); ++i)
                    frame_names.push_back(std::to_string(i));
                  current_frame_item = frame_names[0].c_str();
                }
                if (is_selected)
                {
                  ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
              }
              ImGui::EndCombo();
            }

            //Frames
            if (auto* cur_anim = rigger->GetCurrentAnimation(); cur_anim != nullptr)
            {
              text = std::string("Number of frames ") + std::to_string(rigger->GetCurrentAnimation()->GetNumFrames());
              ImGui::Text(text.c_str());

             //frames combo
              if (ImGui::BeginCombo("Animation frames", current_frame_item)) // The second parameter is the label previewed before opening the combo.
              {
                for (int n = 0; n < frame_names.size(); n++)
                {
                  bool is_selected = (current_frame_item == frame_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
                  if (ImGui::Selectable(frame_names[n].c_str(), is_selected))
                    current_frame_item = frame_names[n].c_str();
                  if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
                ImGui::EndCombo();
              }

              static bool is_frame_freez = false;
              ImGui::Checkbox("FreezeFrame frame", &is_frame_freez);
              if (is_frame_freez)
                rigger->GetCurrentAnimation()->FreezeFrame(std::stoi(current_frame_item));
              else
                rigger->GetCurrentAnimation()->FreezeFrame(-1);
            }

            //Bones
            text = std::string("Number of bones ") + std::to_string(obj->GetRigger()->GetBoneCount());
            ImGui::Text(text.c_str());

            text = std::string("Root Bone: ") + rigger->GetNameRootBone();
            ImGui::Text(text.c_str());

            static bool is_show_active_bone = false;
            ImGui::Checkbox("Show active bone", &is_show_active_bone);

            static glm::mat4 boneMatrix;
            static int bone_current = 0;
            //bones combo
            if (ImGui::BeginCombo("Current bone", current_bone_item)) // The second parameter is the label previewed before opening the combo.
            {
              for (int n = 0; n < bone_names.size(); n++)
              {
                bool is_selected = (current_bone_item == bone_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(bone_names[n].c_str(), is_selected))
                {
                  current_bone_item = bone_names[n].c_str();
                  bone_current = n;
                }
                if (is_selected)
                {
                  ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
              }
              ImGui::EndCombo();
            }

            // parrent bone
            if (const Bone* parent = rigger->GetParent(current_bone_item); parent)
              text = std::string("Parent bone: ") + parent->GetName().c_str();
            else
              text = std::string("Parent bone: none");
            ImGui::Text(text.c_str());
            // children bones
            const auto& children = rigger->GetChildren(current_bone_item);
            if (ImGui::BeginCombo("Children bones :", current_bone_child_item))
            {
              for (int n = 0; n < children.size(); ++n)
              {
                bool is_selected = (current_bone_child_item == children[n]->GetName().c_str());
                if (ImGui::Selectable(children[n]->GetName().c_str(), is_selected))
                {
                  current_bone_child_item = children[n]->GetName().c_str();
                }
                if (is_selected)
                {
                  ImGui::SetItemDefaultFocus();
                }
              }
              ImGui::EndCombo();
            }

            boneMatrix = rigger->GetCurrentMatrixForBone(current_bone_item);
            if (is_show_active_bone)
              rigger->SetActiveBoneIndex(bone_current);
            else
              rigger->SetActiveBoneIndex(MAX_BONES);

            m_combo_list.clear();

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

            /*static glm::mat4 bindMatrix = rigger->GetBindMatrixForBone(current_bone_item);

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
            ImGui::Text(std::to_string(bindMatrix[3][3]).c_str());*/

            //Play animation
            if (ImGui::Button("Play current animations "))
            {
              if (obj->GetRigger()->GetAnimationCount() != 0)
                obj->GetRigger()->Apply(current_animation_item, false);
            }
            if (ImGui::Button("Play once current animations "))
            {
              if (obj->GetRigger()->GetAnimationCount() != 0)
                obj->GetRigger()->Apply(current_animation_item, true);
            }
            if (ImGui::Button("Stop current animations "))
            {
              obj->GetRigger()->Stop();
            }
          }

          // Save Load Animations and Dynamic Collider
          static std::string load_path(150, '\0');
          if (ImGui::InputText("Load Animations from", &load_path[0], 100, ImGuiInputTextFlags_EnterReturnsTrue))
          {

          }
          if (ImGui::Button("Load"))
          {
            if (auto it = callbacks_string.find("Load Rigger"); it != callbacks_string.end())
              it->second(m_current_object, load_path);
          }
          static std::string save_path(150, '\0');
          if (ImGui::InputText("Save Animations to", &save_path[0], 100, ImGuiInputTextFlags_EnterReturnsTrue))
          {

          }
          if (ImGui::Button("Save"))
          {
            if (auto it = callbacks_string.find("Save Rigger"); it != callbacks_string.end())
              it->second(m_current_object, save_path);
          }

          //Socket
          ImGui::Text("Socket");
          auto objects = mp_game->GetObjects();
          if (ImGui::BeginCombo("Game Objects", current_game_object_item))
          {
            for (int n = 0; n < objects.size(); ++n)
            {
              bool is_selected = (current_game_object_item == objects[n]->Name().c_str());
              if (ImGui::Selectable(objects[n]->Name().c_str(), is_selected))
              {
                current_game_object_item = objects[n]->Name().c_str();
              }
              if (is_selected)
              {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
          if (ImGui::Button("Create Socket"))
          {
            if (current_game_object_item)
            {
              shObject child;
              for (auto& obj : objects)
              {
                if (obj->Name() == current_game_object_item)
                  child = obj;
              }
              m_current_object->GetRigger()->CreateSocket(child, current_bone_item);
            }
          }
        }
      }
      break;
      case OBJECT_LIST:
      {
        IGame* p_game = static_cast<IGame*>(std::get<2>(item));
        auto objects = p_game->GetObjects();
        for (auto& obj : objects)
        {
          bool is_visible = obj->IsVisible();
          if(ImGui::Checkbox(obj->Name().c_str(), &is_visible))
          {
            obj->SetVisible(is_visible);
          }
        }
      }
      break;
      case GAME :
      {
        mp_game = static_cast<IGame*>(std::get<2>(item));
      }
      break;
      case SHADER:
      {
        const std::vector<ShaderInfo>* infos = static_cast<const std::vector<ShaderInfo>*>(std::get<2>(item));

        m_combo_list.clear();
        for (size_t i = 0; i < infos->size(); ++i)
        {
          std::string name = (*infos)[i].name;
          for (int j = 0; j < name.size(); ++j)
            m_combo_list.push_back(name[j]);
          if (i != infos->size() - 1)
            m_combo_list.push_back('\0');
        }

        static int shader_current = 0;
        if (!m_combo_list.empty())
        {
          if (ImGui::Combo("Shaders", &shader_current, &m_combo_list[0]))
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
      case CONSOLE:
      {
        std::string result;
        result.resize(150, '\0');
        if (ImGui::InputText(std::get<0>(item).c_str(), &result[0], 100, ImGuiInputTextFlags_EnterReturnsTrue))
        {
          console_callback = *(reinterpret_cast<std::function<void(const std::string&)>*>(std::get<2>(item)));
          console_callback(result);
        }
      }
      break;
      case PARTICLE_SYSTEM:
      {
        std::shared_ptr<IParticleSystem>* p_psystem = static_cast<std::shared_ptr<IParticleSystem>*>(std::get<2>(item));
        if (p_psystem)
        {
          ImGui::SliderFloat("Cone Angle", &p_psystem->get()->ConeAngle(), 0.0f, 2.0f);
          ImGui::SliderFloat("Speed", &p_psystem->get()->Speed(), 0.0f, 1.0f);
          ImGui::SliderFloat("Random Magnitude", &p_psystem->get()->RandomizeMagnitude(), 0.0f, 1.0f);
          ImGui::SliderFloat("Base Radius", &p_psystem->get()->BaseRadius(), 0.0f, 1.0f);
          float scale = p_psystem->get()->Scale().x;
          if (ImGui::SliderFloat("Size", &scale, 0.0f, 1.0f))
          {
            p_psystem->get()->Scale() = glm::vec3{ scale ,scale ,scale };
          }
          ImGui::SliderFloat("Life Length", &p_psystem->get()->LifeLength(), 1.0f, 100.0f);
          ImGui::SliderInt("Particles Per Second", &p_psystem->get()->ParticlesPerSecond(), 1, 100);
          ImGui::SliderFloat("Gravity", &p_psystem->get()->Gravity(), 0.0f, 1.0f);
          ImGui::SliderFloat("Position X", &p_psystem->get()->GetSystemCenter().x, -10.0f, 10.0f);
          ImGui::SliderFloat("Position Y", &p_psystem->get()->GetSystemCenter().y, -10.0f, 10.0f);
          ImGui::SliderFloat("Position Z", &p_psystem->get()->GetSystemCenter().z, -10.0f, 10.0f);
          ImGui::Checkbox("Loop", &p_psystem->get()->Loop());
          ImGui::Image((void*)(intptr_t)(p_psystem->get()->GetTexture()->id), ImVec2(240, 160), ImVec2(0, 1), ImVec2(1, 0));
        }
      }
      break;
      case ADD_CALLBACK:
      {
        std::function<void(shObject, const std::string&)> callback = *(reinterpret_cast<std::function<void(shObject, const std::string&)>*>(std::get<2>(item)));
        callbacks_string.insert({ std::get<0>(item), callback });
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

//-----------------------------------------------------------------
eMainImGuiWindow::eMainImGuiWindow()
  : eWindowImGui("main_window")
{
}

//-----------------------------------------------------------------
void eMainImGuiWindow::Render()
{
  bool open = false, save = false, open_file = false, open_scene = false, save_scene = false;
  ImGui::BeginMainMenuBar();

  ImVec2 pos = ImGui::GetWindowPos();
  window_pos_x = pos.x;
  window_pos_y = pos.y;
  ImVec2 size = ImGui::GetWindowSize();
  window_size_x = size.x;
  window_size_y = size.y;

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
      case MENU_OPEN_SCENE:
      {
        if (ImGui::MenuItem(std::get<0>(item).c_str(), NULL))
        {
          open_scene = true;
          open_scene_menu_name = std::get<0>(item);
          open_scene_callback = *(reinterpret_cast<std::function<void(const std::string&)>*>(std::get<2>(item)));
        }
      }
      break;
      case MENU_SAVE_SCENE:
      {
        if (ImGui::MenuItem(std::get<0>(item).c_str(), NULL))
        {
          save_scene = true;
          save_scene_menu_name = std::get<0>(item);
          save_scene_callback = *(reinterpret_cast<std::function<void(const std::string&)>*>(std::get<2>(item)));
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
  if (open_file)
  {
    ImGui::OpenPopup(open_file_menu_name.c_str());
    eImGuiContext::SetBlockEvents(true);
  }
  if (open_scene)
  {
    ImGui::OpenPopup(open_scene_menu_name.c_str());
    eImGuiContext::SetBlockEvents(true);
  }
  if (save_scene)
  {
    ImGui::OpenPopup(save_scene_menu_name.c_str());
    eImGuiContext::SetBlockEvents(true);
  }

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
  if (file_dialog.showFileDialog(open_file_menu_name.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 200), ".obj"))
  {
    open_file_callback(file_dialog.selected_path);
    eImGuiContext::SetBlockEvents(false);
  }
  if (file_dialog.showFileDialog(open_scene_menu_name.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 200), ".mgoongaScene"))
  {
    open_scene_callback(file_dialog.selected_path);
    eImGuiContext::SetBlockEvents(false);
  }
  if (file_dialog.showFileDialog(save_scene_menu_name.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 200), ".mgoongaScene"))
  {
    save_scene_callback(file_dialog.selected_path);
    eImGuiContext::SetBlockEvents(false);
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

//-------------------------------------------------------------------
bool eMainImGuiWindow::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
  cursor_x = _x;
  cursor_y = _y;
  if (IsHovered())
    return true;
  else
    return false;
}

//-------------------------------------------------------------------
bool eMainImGuiWindow::IsHovered()
{
  float global_pos_x = cursor_x + viewport_offset_x + window_offset_x;
  float global_pos_y = cursor_y + viewport_offset_y + window_offset_y;
  return global_pos_x > window_pos_x &&
    global_pos_y > window_pos_y &&
    global_pos_x < (window_pos_x + window_size_x) &&
    global_pos_y < (window_pos_y + window_size_y + 20);
}
