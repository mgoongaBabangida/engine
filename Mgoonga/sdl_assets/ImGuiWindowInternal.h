#pragma once

#include "ImGuiContext.h"

//----------------------------------------------------
class DLL_SDL_ASSETS eImGuiWindowInternal : public eWindowImGui
{
public:
  explicit eImGuiWindowInternal(int _image_id);
  virtual void Render() override;
protected:
  int m_image_id = -1;
};