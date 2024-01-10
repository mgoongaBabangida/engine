#include "stdafx.h"
#include "ImGuiWindowInternal.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include <sdl_assets/font_awesome.h>
#include <sdl_assets/font_awesome.cpp>

//xo1337

inline ImFont* edwardian_font = nullptr;
inline ImFont* icons_font = nullptr;

//-------------------------------------------------------------------
inline void CenterButtons(const std::vector<std::string>& names, const std::vector<int>& indexes, int& selected_index)
{
  std::vector<ImVec2> sizes = {};
  float total_area = 0.0f;

  const auto& style = ImGui::GetStyle();
  for (auto& name: names)
  {
    const ImVec2 label_size = ImGui::CalcTextSize(name.c_str(), 0, true);
    ImVec2 size = ImGui::CalcItemSize(ImVec2(), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
    size.x += 45.5f;
    size.y += 15.0f;

    sizes.push_back(size);
    total_area += size.x;
  }

  ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (total_area / 2));
  for (uint32_t i = 0; i < names.size(); ++i)
  {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 70);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    if (selected_index == indexes[i])
    {
      ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0,189,0,255).Value);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(0, 189, 0, 255).Value);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0, 189, 0, 255).Value);
      if (ImGui::Button(names[i].c_str(), sizes[i]))
        selected_index = indexes[i];
      ImGui::PopStyleColor(3);
    }
    else
    {
      if (ImGui::Button(names[i].c_str(), sizes[i]))
        selected_index = indexes[i];
    }
    ImGui::PopStyleVar();
    if (i != names.size() - 1)
      ImGui::SameLine();
  }
}

//------------------------------------------------------------------
inline void Centertext(const char* format, const float y_padding = 0.0f, ImColor color = ImColor(255, 255, 255))
{
  const ImVec2 text_size = ImGui::CalcTextSize(format);
  ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (text_size.x / 2));
  if(y_padding == 0.0f)
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_padding);
  ImGui::TextColored(color, format);
}

//------------------------------------------------------------------
inline void CustomCheckBox(const char* format, bool* value)
{
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.5f,1.5f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0,189,0,255).Value);
  ImGui::Checkbox(format, value);
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);
  ImGui::Dummy(ImVec2());
}

//--------------------------------------------------------------------------
void SetCustomStyle()
{
  //Style
  ImGuiStyle* style = &ImGui::GetStyle();

  style->ScrollbarRounding = 0;
  style->WindowRounding = 4.0f;

  /*style->WindowBorderSize = 0;
  style->WindowTitleAlign = ImVec2(0.5, 0.5);
  style->WindowMinSize = ImVec2(900, 430);*/

  /* style->FramePadding = ImVec2(8, 6);*/

  /* style->Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
   style->Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
   style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);*/

  style->Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 255);
  style->Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 0, 0, 255);
  style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 0, 0, 255);

  style->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
  style->Colors[ImGuiCol_ButtonActive] = ImColor(31, 30, 31, 255);
  style->Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);

  style->Colors[ImGuiCol_CheckMark] = ImColor(0, 189, 0, 255);

  /* style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
   style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
   style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);*/

  style->Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
  style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
  style->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);

  /*style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
  style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
  style->Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);*/
}

//------------------------------------------------------------------
eImGuiWindowInternal::eImGuiWindowInternal(int _image_id)
  :m_image_id(_image_id)
{
  //Load font
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();

  edwardian_font = io.Fonts->AddFontFromFileTTF("../game_assets/fonts/edwardianscriptitc.ttf", 16.0f);

  //icon config
  static const ImWchar icon_ranges[]{ 0xf000, 0xf3ff, 0 };
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.OversampleH = 3;
  icons_config.OversampleV = 3;

  icons_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.5f, &icons_config, icon_ranges);
}

//--------------------------------------------------------------------------
void eImGuiWindowInternal::Render()
{
  static int index = 0;

  SetCustomStyle();

  ImGui::SetNextWindowSize({ 635, 435 });
  ImGui::Begin("GuiderHacking", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

  ImVec2 window_pos = ImGui::GetWindowPos();
  window_pos_x = window_pos.x;
  window_pos_y = window_pos.y;
  ImVec2 wndow_size = ImGui::GetWindowSize();
  window_size_x = wndow_size.x;
  window_size_y = wndow_size.y;
  ImVec2 cursor_pos = ImGui::GetCursorPos();

  ImVec2 image_size{ 770/ 2, 78 };

  ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (image_size.x / 2));
  ImGui::Image((void*)(intptr_t)(m_image_id), image_size, ImVec2(0, 1), ImVec2(1, 0));

  ImGui::PushFont(edwardian_font);
  /*ImGui::Text("Edwardian!");
  ImGui::PopFont();*/

  ImGui::PushFont(icons_font);
  /*ImGui::Text(ICON_FA_BOOK);
  ImGui::PopFont();*/

  CenterButtons({ ICON_FA_CROSSHAIRS" Aimbot", ICON_FA_USERS" Visuals", ICON_FA_COGS" Other" }, { 0,1,2 }, index);

  ImGui::PushStyleColor(ImGuiCol_Separator, ImColor(0, 189 ,0).Value);
  ImGui::Separator();
  ImGui::PopStyleColor();

  ImGui::BeginChild("##left_side", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, ImGui::GetContentRegionAvail().y));
  {
    static bool checkbox = false;
    ImGui::Checkbox("checkbox style", &checkbox);
  }
  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::PushStyleColor(ImGuiCol_Separator, ImColor(0, 189, 0).Value);
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
  ImGui::PopStyleColor();

  ImGui::PopFont();
  ImGui::PopFont();

  ImGui::GetStyle() = ImGuiStyle();

  ImGui::End();
}
//there is also animation of borders and alpha of a text in //xo1337
