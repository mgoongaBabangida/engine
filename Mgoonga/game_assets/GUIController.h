#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

#include <math/Rect.h>

class eMainContextBase;
class eOpenGlRenderPipeline;
class RemSnd;

//---------------------------------------------------------
class DLL_GAME_ASSETS GUIControllerBase : public IScript
{
public:
  GUIControllerBase(eMainContextBase*, eOpenGlRenderPipeline&, RemSnd*);

  virtual void Initialize() override;
  virtual void	Update(float _tick) override {}
protected:
  eMainContextBase* m_game = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  std::vector<std::shared_ptr<eObject>> m_objects;
  RemSnd* m_page_sound = nullptr;
};

//---------------------------------------------------------
class DLL_GAME_ASSETS GUIController : public GUIControllerBase
{
public:
  GUIController(eMainContextBase*, eOpenGlRenderPipeline&, RemSnd*);
  virtual ~GUIController();

  virtual void Initialize() override;
  virtual void	Update(float _tick) override;
  virtual bool  OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifier) override;

  float m_cursor_x = 0.0f;
  float m_cursor_y = 0.0f;
  bool m_is_menu_active = true;
  bool m_cursor_is_outside_buttons = true;
  std::vector<dbb::Rect> m_buttons;
};
