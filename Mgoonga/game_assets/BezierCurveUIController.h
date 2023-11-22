#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Event.h>

#include <math/Bezier.h>
#include <array>

class eObject;
class BezierCurveMesh;
class eMainContextBase;
struct Texture;
class GUI;

//---------------------------------------------------------
class DLL_GAME_ASSETS BezierCurveUIController : public IScript
{
public:
  explicit BezierCurveUIController(eMainContextBase* _game,
                                   std::shared_ptr<eObject> _bezier_object,
                                   float _control_point_size,
                                   const Texture* _window = nullptr);
  virtual ~BezierCurveUIController();

  Event<std::function<void(const dbb::Bezier&)>> ToolFinished;

  virtual void	Update(float _tick) override;
  virtual bool	OnMouseMove(int32_t x, int32_t y, KeyModifiers _modifiers) override;

protected:
  eMainContextBase* m_game = nullptr;
  std::shared_ptr<GUI> m_window;

  dbb::Bezier* m_bezier;
  const BezierCurveMesh* m_bezier_mesh;
  float m_cursor_x;
  float m_cursor_y;
  float m_closed = false;
};