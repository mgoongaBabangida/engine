#pragma once

#include "game_assets.h"

#include <base/Object.h>
#include <base/Event.h>

#include <math/Bezier.h>
#include <array>

class BezierCurveMesh;
class eMainContextBase;
struct Texture;
class GUI;

//---------------------------------------------------------
class DLL_GAME_ASSETS BezierCurveUIController : public IScript
{
public:
  explicit BezierCurveUIController(eMainContextBase* _game,
                                   shObject _bezier_object,
                                   float _control_point_size,
                                   const Texture* _window = nullptr);
  virtual ~BezierCurveUIController();

  Event<std::function<void(shObject)>> ToolFinished;

  virtual void	Update(float _tick) override;
  virtual bool	OnMouseMove(int32_t x, int32_t y) override;

protected:
  eMainContextBase* m_game = nullptr;
  std::shared_ptr<GUI> m_window;

  shObject m_bezier_object;
  dbb::Bezier* m_bezier;
  const BezierCurveMesh* m_bezier_mesh;
  float m_cursor_x;
  float m_cursor_y;
};