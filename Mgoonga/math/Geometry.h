#pragma once

#include "math.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

namespace dbb
{
  using triangle = glm::mat3;
  using point = glm::vec3;

  //-------------------------------------------------
  struct lineSegment
  {
    dbb::point start;
    dbb::point end;
    inline lineSegment() {}
    inline lineSegment(const dbb::point& s, const dbb::point& e) :
      start(s), end(e) { }

    float Length();
    float LengthSq();
  };

  //-------------------------------------------------
  struct ray
  {
    dbb::point origin;
    glm::vec3 direction;

    inline ray() : direction(0.0f, 0.0f, 1.0f), origin(0.0f, 0.0f, 0.0f) {}
    inline ray(const dbb::point& o, const glm::vec3& d) :
      origin(o), direction(d) {
      NormalizeDirection();
    }

    inline void NormalizeDirection() {
      glm::normalize(direction);
    }
  };

  dbb::ray inline FromPoint(const dbb::point& from, const dbb::point& to)
  {
    return ray(from, glm::normalize(to - from));
  }

  //-------------------------------------------------
  struct sphere
  {
    dbb::point position;
    float radius;

    inline sphere() : radius(1.0f) { }
    inline sphere(const dbb::point& p, float r) :
      position(p), radius(r) { }
  };

  //----------------------------------------------------
  struct AABB
  {
    dbb::point origin;
    glm::vec3 size;

    inline AABB() : size(1, 1, 1) { }
    inline AABB(const dbb::point& o, const glm::vec3& s) :
      origin(o), size(s) { }

    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
  };

  AABB FromMinMax(const glm::vec3& min, const glm::vec3& max);

  //----------------------------------------------------
  struct OBB
  {
    dbb::point origin;
    glm::vec3 size;
    glm::mat3 orientation;
  
    inline OBB() : size(1, 1, 1) { }
    inline OBB(const dbb::point& p, const glm::vec3& s) :
      origin(p), size(s) { }
    inline OBB(const dbb::point& p, const glm::vec3& s, const glm::mat3& o)
      : origin(p), size(s), orientation(o) { }
  };

  bool IsPointInSphere(const dbb::point& point, const dbb::sphere& sphere);
  dbb::point GetClosestPointOnSphere(const dbb::sphere& sphere, const dbb::point& point);
  bool IsPointInAABB(const dbb::point& point, const AABB& aabb);
  dbb::point GetClosestPointOnAABB(const AABB& aabb, const dbb::point& point);
  bool IsPointInOBB(const dbb::point& point, const OBB& obb);
  dbb::point GetClosestPointOnOBB(const OBB& obb, const dbb::point& point);
  bool IsPointOnLine(const dbb::point& point, const dbb::lineSegment& line);
  dbb::point GetClosestPointOnLineSegment(const dbb::lineSegment& line, const dbb::point& point);
  bool IsPointOnRay(const dbb::point& point, const dbb::ray& ray);
  dbb::point GetClosestPointOnRay(const dbb::ray& ray, const dbb::point& point);
}