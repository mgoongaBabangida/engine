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
  explicit BezierCurveUIController(const std::array<shObject, 5>& _bezier_objects);
  virtual ~BezierCurveUIController();

  virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override;
protected:
  std::array<shObject, 5> m_bezier_objects;
  dbb::Bezier* m_bezier;
  const BezierCurveMesh* m_bezier_mesh;
};