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

  //-------------------------------------------------------------
  bool SphereSphere(const dbb::sphere& s1, const dbb::sphere& s2)
  {
    float radiiSum = s1.radius + s2.radius;
    float sqDistance = glm::length2(s1.position - s2.position);
    return sqDistance < radiiSum * radiiSum;
  }

  //-----------------------------------------------------------
  bool SphereAABB(const dbb::sphere& sphere, const AABB& aabb)
  {
    dbb::point closestPoint = GetClosestPointOnAABB(aabb, sphere.position);
    float distSq = glm::length2(sphere.position - closestPoint);
    float radiusSq = sphere.radius * sphere.radius;
    return distSq < radiusSq;
  }

  //-----------------------------------------------------------
  bool SphereOBB(const dbb::sphere& sphere, const OBB& obb)
  {
    dbb::point closestPoint = GetClosestPointOnOBB(obb, sphere.position);
    float distSq = glm::length2(sphere.position - closestPoint);
    float radiusSq = sphere.radius * sphere.radius;
    return distSq < radiusSq;
  }

  //---------------------------------------------------------
  bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
  {
    dbb::point aMin = aabb1.GetMin();
    dbb::point aMax = aabb1.GetMax();
    dbb::point bMin = aabb2.GetMin();
    dbb::point bMax = aabb2.GetMax();
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
  }

  //--------------------------------------------------------------
  Interval GetInterval(const AABB& aabb, const glm::vec3& axis)
  {
   glm::vec3 i = aabb.GetMin();
   glm::vec3 a = aabb.GetMax();
   glm::vec3 vertex[8] = {
   glm::vec3(i.x, a.y, a.z),
   glm::vec3(i.x, a.y, i.z),
   glm::vec3(i.x, i.y, a.z),
   glm::vec3(i.x, i.y, i.z),
   glm::vec3(a.x, a.y, a.z),
   glm::vec3(a.x, a.y, i.z),
   glm::vec3(a.x, i.y, a.z),
   glm::vec3(a.x, i.y, i.z)
    };

   Interval result;
   result.min = result.max = glm::dot(axis, vertex[0]);
   for (int i = 1; i < 8; ++i)
   {
     float projection = glm::dot(axis, vertex[i]);
     result.min = (projection < result.min) ?
       projection : result.min;
     result.max = (projection > result.max) ?
       projection : result.max;
   }
   return result;
  }

  //-----------------------------------------------------------
  Interval GetInterval(const OBB& obb, const glm::vec3& axis)
  {
    glm::vec3 vertex[8];
    glm::vec3 C = obb.origin; // OBB Center
    glm::vec3 E = obb.size; // OBB Extents
    const float* o = &obb.orientation[0][0];
    glm::vec3 A[] = { // OBB Axis
    glm::vec3(o[0], o[1], o[2]),
    glm::vec3(o[3], o[4], o[5]),
    glm::vec3(o[6], o[7], o[8]),
    };

    vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

    Interval result;
    result.min = result.max = glm::dot(axis, vertex[0]);
    for (int i = 1; i < 8; ++i)
    {
      float projection = glm::dot(axis, vertex[i]);
      result.min = (projection < result.min) ?
        projection : result.min;
      result.max = (projection > result.max) ?
        projection : result.max;
    }
    return result;
  }

  //-------------------------------------------------------------------------
  bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis)
  {
    Interval a = GetInterval(aabb, axis);
    Interval b = GetInterval(obb, axis);
    return ((b.min <= a.max) && (a.min <= b.max));
  }

  //-------------------------------------------------
  bool AABBOBB(const AABB& aabb, const OBB& obb)
  {
    const float* o = &obb.orientation[0][0];
    glm::vec3 test[15] = {
    glm::vec3(1, 0, 0), // AABB axis 1
    glm::vec3(0, 1, 0), // AABB axis 2
    glm::vec3(0, 0, 1), // AABB axis 3
    glm::vec3(o[0], o[1], o[2]), // OBB axis 1
    glm::vec3(o[3], o[4], o[5]), // OBB axis 2
    glm::vec3(o[6], o[7], o[8]) // OBB axis 3
    // We will fill out the remaining axis in the next step
    };

    for (int i = 0; i < 3; ++i) { // Fill out rest of axis
      test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
      test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
      test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    for (int i = 0; i < 15; ++i) {
      if (!OverlapOnAxis(aabb, obb, test[i])) {
        return false; // Seperating axis found
      }
    }
    return true; // Seperating axis not found
  }

  //-------------------------------------------------------------------------
  bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis)
  {
    Interval a = GetInterval(obb1, axis);
    Interval b = GetInterval(obb1, axis);
    return ((b.min <= a.max) && (a.min <= b.max));
  }

  //---------------------------------------------------------
  bool OBBOBB(const OBB& obb1, const OBB& obb2)
  {
    const float* o1 = &obb1.orientation[0][0];
    const float* o2 = &obb2.orientation[0][0];
    glm::vec3 test[15] = {
    glm::vec3(o1[0], o1[1], o1[2]),
    glm::vec3(o1[3], o1[4], o1[5]),
    glm::vec3(o1[6], o1[7], o1[8]),
    glm::vec3(o2[0], o2[1], o2[2]),
    glm::vec3(o2[3], o2[4], o2[5]),
    glm::vec3(o2[6], o2[7], o2[8])
    };

    for (int i = 0; i < 3; ++i)
    { // Fill out rest of axis
      test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
      test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
      test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    for (int i = 0; i < 15; ++i) {
      if (!OverlapOnAxis(obb1, obb2, test[i])) {
        return false; // Seperating axis found
      }
    }
    return true; // Seperating axis not found
  }
}
