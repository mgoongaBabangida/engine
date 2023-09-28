#include "Bezier.h"

//---------------------------------------------------------
glm::vec3 dbb::GetPoint(const Bezier& _bezier, float u)
{
 /* glm::vec3 out;
  out = _bezier.p0 * (-(_t * _t * _t) + 3 * (_t * _t) + 1.0f)
    + _bezier.p1 * (3 * (_t * _t * _t) - 6 * (_t * _t) + 3 * _t)
    + _bezier.p2 * (-3 * (_t * _t * _t) + 3 * (_t * _t))
    + _bezier.p3 * (_t * _t * _t);
  return out;*/

  float u1 = (1.0 - u);
  float u2 = u * u;

  float b3 = u2 * u;
  float b2 = 3.0 * u2 * u1;
  float b1 = 3.0 * u * u1 * u1;
  float b0 = u1 * u1 * u1;

  return _bezier.p0 * b0 + _bezier.p1 * b1 + _bezier.p2 * b2 + _bezier.p3 * b3;
}