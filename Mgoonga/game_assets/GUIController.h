#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

class eMainContextBase;
class eOpenGlRenderPipeline;
class RemSnd;
namespace dbb
{
  struct Rect;
}

//---------------------------------------------------------
class DLL_GAME_ASSETS GUIController : public IScript
{
public:
  GUIController(eMainContextBase*, eOpenGlRenderPipeline&, RemSnd*);
  virtual ~GUIController();

  virtual void	Update(std::vector<std::shared_ptr<eObject>> _objs) override;
  virtual bool OnMouseMove(int32_t _x, int32_t _y) override;

protected:
  eMainContextBase* m_game = nullptr;
  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  std::vector<std::shared_ptr<eObject>> m_objects;

  float m_cursor_x = 0.0f;
  float m_cursor_y = 0.0f;
  bool m_is_menu_active = true;
  bool m_cursor_is_outside_buttons = true;
  std::vector<dbb::Rect> m_buttons;
  RemSnd* m_page_sound = nullptr;
};
