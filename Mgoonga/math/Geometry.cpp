#include "stdafx.h"
#include "Geometry.h"

#include <cmath>
#include <cfloat>

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

namespace dbb
{
  //----------------------------------------------------
  float lineSegment::Length()
  {
    return glm::length(start - end);
  }

  //----------------------------------------------------
  float lineSegment::LengthSq()
  {
    return glm::length2(start - end);
  }

  //----------------------------------------------------
  glm::vec3 AABB::GetMin() const
  {
    glm::vec3 p1 = origin + size;
    glm::vec3 p2 = origin - size;
    return  glm::vec3(fminf(p1.x, p2.x),
                      fminf(p1.y, p2.y),
                      fminf(p1.z, p2.z));
  }

  //----------------------------------------------------
  glm::vec3 AABB::GetMax() const
  {
    glm::vec3 p1 = origin + size;
    glm::vec3 p2 = origin - size;
    return glm::vec3(fmaxf(p1.x, p2.x),
                     fmaxf(p1.y, p2.y),
                     fmaxf(p1.z, p2.z));
  }

  //----------------------------------------------------
  AABB FromMinMax(const glm::vec3& min, const glm::vec3& max)
  {
    return AABB((min + max) * 0.5f, (max - min) * 0.5f);
  }

  //--------------------------------------------------------------------
  bool IsPointInSphere(const dbb::point& point, const dbb::sphere& sphere)
  {
    float magSq = glm::length2(point - sphere.position);
    float radSq = sphere.radius * sphere.radius;
    return magSq < radSq;
  }

  //---------------------------------------------------------------------
  dbb::point GetClosestPointOnSphere(const dbb::sphere& sphere, const dbb::point& point)
  {
    glm::vec3 sphereToPoint = point - sphere.position;
    glm::normalize(sphereToPoint);
    sphereToPoint = sphereToPoint * sphere.radius;
    return sphereToPoint + sphere.position;
  }

  //---------------------------------------------------------------------
  bool IsPointInAABB(const dbb::point& point, const AABB& aabb)
  {
    dbb::point min = aabb.GetMin();
    dbb::point max = aabb.GetMax();
    if (point.x < min.x || point.y < min.y || point.z < min.z)
      return false;
    if (point.x > max.x || point.y > max.y || point.z > max.z)
      return false;
    return true;
  }

  //---------------------------------------------------------------------
  dbb::point GetClosestPointOnAABB(const AABB& aabb, const dbb::point& point)
  {
    dbb::point result = point;
    dbb::point min = aabb.GetMin();
    dbb::point max = aabb.GetMax();
    result.x = (result.x < min.x) ? min.x : result.x;
    result.y = (result.y < min.x) ? min.y : result.y;
    result.z = (result.z < min.x) ? min.z : result.z;
    result.x = (result.x > max.x) ? max.x : result.x;
    result.y = (result.y > max.x) ? max.y : result.y;
    result.z = (result.z > max.x) ? max.z : result.z;
    return result;
  }

  //---------------------------------------------------------------------
  bool IsPointInOBB(const dbb::point& point, const OBB& obb)
  {
    glm::vec3 dir = point - obb.origin;
    for (int i = 0; i < 3; ++i)
    {
      const float* orientation = &obb.orientation[i * 3][0];
      glm::vec3 axis(orientation[0], orientation[1],orientation[2]);
      float distance = glm::dot(dir, axis);
      if (distance > obb.size[i])
        return false;
      if (distance < -obb.size[i])
        return false;
    }
    return true;
  }

  //---------------------------------------------------------------------
  dbb::point GetClosestPointOnOBB(const OBB& obb, const dbb::point& point)
  {
    dbb::point result = obb.origin;
    glm::vec3 dir = point - obb.origin;
    for (int i = 0; i < 3; ++i)
    {
      const float* orientation = &obb.orientation[i * 3][0];;
      glm::vec3 axis( orientation[0], orientation[1],orientation[2]);
      float distance = glm::dot(dir, axis);
      if (distance > obb.size[i])
        distance = obb.size[i];
      if (distance < -obb.size[i])
        distance = -obb.size[i];
      result = result + (axis * distance);
    }
    return result;
  }

  //----------------------------------------------------------------------
  bool IsPointOnLine(const dbb::point& point, const dbb::lineSegment& line)
  {
    dbb::point closest = GetClosestPointOnLineSegment(line, point);
    float distanceSq = glm::length2(closest - point);
    // Consider using an epsilon test here!
    // CMP(distanceSq, 0.0f);
    return distanceSq == 0.0f;
  }

  //----------------------------------------------------------------------
  dbb::point GetClosestPointOnLineSegment(const dbb::lineSegment& line, const dbb::point& point)
  {
    glm::vec3 lVec = line.end - line.start; // Line Vector
    float t = glm::dot(point - line.start, lVec) / glm::dot(lVec, lVec);
    t = fmaxf(t, 0.0f); // Clamp to 0
    t = fminf(t, 1.0f); // Clamp to 1
    return line.start + lVec * t;
  }

  //----------------------------------------------------------------------
  bool IsPointOnRay(const dbb::point& point, const dbb::ray& ray)
  {
    if (point == ray.origin)
      return true;

    glm::vec3 norm = point - ray.origin;
    glm::normalize(norm);
    // We assume the ray direction is normalized
    float diff = glm::dot(norm, ray.direction);
    // If BOTH vectors point in the same direction, 
    // their dot product (diff) should be 1
    return diff == 1.0f; // Consider using epsilon!
  }

  //----------------------------------------------------------------------
  dbb::point GetClosestPointOnRay(const dbb::ray& ray, const dbb::point& point)
  {
    float t = glm::dot(point - ray.origin, ray.direction);
    // We assume the direction of the ray is normalized
    // If for some reason the direction is not normalized
    // the below division is needed. So long as the ray 
    // direction is normalized, we don't need this divide
    // t /= Dot(ray.direction, ray.direction);
    t = fmaxf(t, 0.0f);
    return dbb::point(ray.origin + ray.direction * t);
  }
}
