#pragma once

#include "game_assets.h"

#include <base/Object.h>
#include <math/Bezier.h>
#include <array>

class BezierCurveMesh;

//---------------------------------------------------------
class DLL_GAME_ASSETS BezierCurveUIController : public IScript
{
public:
  explicit BezierCurveUIController(shObject _bezier_object);
  virtual ~BezierCurveUIController();

  virtual void	Update(float _tick) override;
protected:
  shObject m_bezier_object;
  dbb::Bezier* m_bezier;
  const BezierCurveMesh* m_bezier_mesh;
};