#pragma once

#include "math.h"

#include <glm\glm\glm.hpp>

namespace dbb
{
  struct Bezier
  {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
  };

  glm::vec3 GetPoint(const Bezier& _bezier, float _t); // t is [0 -1]

  //float GetArcLength(const Bezier& _bezier)
  //{
  //  //@todo
  //  return 0;
  //}

  //glm::vec3 GetPointAtDistance(const Bezier& _bezier, float _distance)
  //{
  //  //Arclength parametrization
  //  // // Use LUT distance is [0 arcLength]
  //  //@todo
  //  float arcLength = GetArcLength(_bezier);
  //  float t = _distance / arcLength;
  //  return GetPoint(_bezier, t);
  //}

  // Derivative of cubic bezier is cuadratic bezier (velocity of the point) (get Tangent and Normal), useful for generating roads in 3d
  // Second derivative is acceleration of the point (linear bezier curve or just line segment)
  // Third derivative is Jerk is rate of change of acceleration(this is a point)

  //Curvature Det(P1,P2)/(||P1||*||P1||*||P1||); how bent it is at a given point
  //With derivateve can get bounding box of bezier curve

}
