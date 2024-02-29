#include "stdafx.h"
#include "Geometry.h"

#include <cmath>
#include <cfloat>

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

namespace dbb
{
  //----------------------------------------------------
  float lineSegment::Length() const
  {
    return glm::length(start - end);
  }

  //----------------------------------------------------
  float lineSegment::LengthSq() const
  {
    return glm::length2(start - end);
  }

  //----------------------------------------------------
  bool lineSegment::LineTest(const dbb::sphere& sphere) const
  {
    dbb::point closest; // = GetClosestPointOnLineSegment(*this, sphere.position); @todo !!!!!!!!!!!!!!
    float distSq = glm::length2(sphere.position - closest);
    return distSq <= (sphere.radius * sphere.radius);
  }

  //----------------------------------------------------
  bool lineSegment::LineTest(const AABB& aabb) const
  {
    dbb::ray ray;
    ray.origin = start;
    ray.direction = glm::normalize(end - start);
    RaycastResult raycast;
    float t = ray.Raycast(aabb, raycast);
    if (!raycast.hit)
      return false;
    return t >= 0 && t * t <= LengthSq();
  }

  //----------------------------------------------------
  bool lineSegment::LineTest(const OBB& obb) const
  {
    dbb::ray ray;
    ray.origin = start;
    ray.direction = glm::normalize(end - start);
    RaycastResult raycast;
    float t = ray.Raycast(obb, raycast);
    if (!raycast.hit)
      return false;
    return t >= 0 && t * t <= LengthSq();
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

  //---------------------------------------------------------------------------------
  float ray::Raycast(const dbb::sphere& sphere, RaycastResult& outResult)
  {
    RaycastResult::ResetRaycastResult(&outResult);
    //Construct a vector from the origin of the ray to the center of the sphere:
    glm::vec3 e = sphere.position - origin;
    //Store the squared magnitude of this new vector, as well as the squared radius of the sphere :
    float rSq = sphere.radius * sphere.radius;
    float eSq = glm::length2(e);
    // ray.direction is assumed to be normalized
    float a = glm::dot(e, glm::normalize(direction));
   /* Construct the sides a triangle using the radius of the circle at the projected point
      from the last step.The sides of this triangle are radius, band f.We work with
      squared units :*/
    float bSq = eSq - (a * a);
    float f = sqrt(rSq - bSq);
    float t = a - f; // Assume normal intersection!
    /*Compare the length of the squared radius against the hypotenuse of the triangle from
      the last step.This is visually explained in the How it works section :*/
    // No collision has happened
    if (rSq - (eSq - (a * a)) < 0.0f)
      return -1; // -1 is invalid.

    // Ray starts inside the sphere
    else if (eSq < rSq)
      return a + f; // Just reverse direction

    outResult.t = t;
    outResult.hit = true;
    outResult.point = origin + direction * t;
    outResult.normal = glm::normalize(outResult.point - sphere.position);

    // else Normal intersection
    return a - f;
  }

  //----------------------------------------------------
  float ray::Raycast(const AABB& aabb, RaycastResult& outResult)
  {
    RaycastResult::ResetRaycastResult(&outResult);
    glm::vec3 min = aabb.GetMin();
    glm::vec3 max = aabb.GetMax();
    // NOTE: Any component of direction could be 0!
    // to avoid a division by 0, you need to add 
    // additional safety checks.
    float t[] = { 0, 0, 0, 0, 0, 0 };
    t[0] = (min.x - origin.x) / direction.x;
    t[1] = (max.x - origin.x) / direction.x;
    t[2] = (min.y - origin.y) / direction.y;
    t[3] = (max.y - origin.y) / direction.y;
    t[4] = (min.z - origin.z) / direction.z;
    t[5] = (max.z - origin.z) / direction.z;
    //Find the largest minimum value
    float tmin = fmaxf(
      fmaxf(
        fminf(t[0], t[1]),
        fminf(t[2], t[3])
      ),
      fminf(t[4], t[5])
    );
    //Find the smallest maximum value
    float tmax = fminf(
      fminf(
        fmaxf(t[0], t[1]),
        fmaxf(t[2], t[3])
      ),
      fmaxf(t[4], t[5])
    );

    /*If tmax is less than zero, the ray is intersecting AABB in the negative direction.This
  means the entire AABB is behind the origin of the ray, this should not be treated as
  an intersection :*/
    if (tmax < 0)
      return -1;
    /* If tmin is greater than tmax, the ray does not intersect AABB :*/
    if (tmin > tmax)
      return -1;

    float t_result = tmin;
    if (tmin < 0.0f) { t_result = tmax; }

    outResult.t = t_result;
    outResult.hit = true;
    outResult.point = origin + direction * t_result;
    glm::vec3 normals[] = {
    glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0),
    glm::vec3(0, -1, 0), glm::vec3(0, 1, 0),
    glm::vec3(0, 0, -1), glm::vec3(0, 0, 1)
    };
    for (int i = 0; i < 6; ++i) {
      if (t_result == t[i])
        outResult.normal = normals[i];
    }

    /*If tmin is less than zero, that means the ray intersects the AABB but its origin is
      inside the AABB.This means tmax is the valid collision point :*/
    if (tmin < 0.0f)
      return tmax;
    return tmin;
  }

  //------------------------------------------
  float ray::Raycast(const OBB& obb, RaycastResult& outResult)
  {
    RaycastResult::ResetRaycastResult(&outResult);
    const float* o = &obb.orientation[0][0];
    const float* size = &obb.size[0];
    // X, Y and Z axis of OBB
    glm::vec3 X(o[0], o[1], o[2]);
    glm::vec3 Y(o[3], o[4], o[5]);
    glm::vec3 Z(o[6], o[7], o[8]);
    glm::vec3 p = obb.origin - origin;
    glm::vec3 f(
     glm::dot(X, direction),
     glm::dot(Y, direction),
     glm::dot(Z, direction)
    );
    glm::vec3 e(
      glm::dot(X, p),
      glm::dot(Y, p),
      glm::dot(Z, p)
    );
    float t[6] = { 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 3; ++i)
    {
      if (f[i] == 0) //CMP ?
      {
        if (-e[i] - size[i] > 0 || -e[i] + size[i] < 0)
          return -1;
        f[i] = 0.00001f; // Avoid div by 0!
      }
      t[i * 2 + 0] = (e[i] + size[i]) / f[i]; // min
      t[i * 2 + 1] = (e[i] - size[i]) / f[i]; // max
    }

    float tmin = fmaxf(
      fmaxf(
        fminf(t[0], t[1]),
        fminf(t[2], t[3])),
      fminf(t[4], t[5])
    );
    float tmax = fminf(
      fminf(
        fmaxf(t[0], t[1]),
        fmaxf(t[2], t[3])),
      fmaxf(t[4], t[5])
    );

    if (tmax < 0)
      return -1.0f;
    if (tmin < 0.0f)
      return tmax;

    float t_result = tmin;
    if (tmin < 0.0f) { t_result = tmax; }

    outResult.hit = true;
    outResult.t = t_result;
    outResult.point = origin + direction
      * t_result;
    glm::vec3 normals[] = { X, X * -1.0f,
    Y, Y * -1.0f,
    Z, Z * -1.0f
    };
    for (int i = 0; i < 6; ++i) {
      if (t_result == t[i])
        outResult.normal = glm::normalize(normals[i]);
    }

    return tmin;
  }

  //------------------------------------------------------------
  void CollisionManifold::ResetCollisionManifold(CollisionManifold& result)
  {
    result.colliding = false;
    result.normal = glm::vec3(0, 0, 1);
    result.depth = FLT_MAX;
    result.contacts.clear();
  }

  //------------------------------------------------------------
  std::vector<dbb::point> OBB::GetVertices() const
  {
    std::vector<glm::vec3> v;
    v.resize(8);
    glm::vec3 C = origin; // OBB Center
    glm::vec3 E = size; // OBB Extents
    const float* o = &orientation[0][0];
    glm::vec3 A[] = { // OBB Axis
    glm::vec3(o[0], o[1], o[2]),
    glm::vec3(o[3], o[4], o[5]),
    glm::vec3(o[6], o[7], o[8]),
    };
    v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    return v;
  }

  //------------------------------------------------------------
  std::vector<dbb::lineSegment> OBB::GetEdges() const
  {
    std::vector<dbb::lineSegment> result;
    result.reserve(12);
    std::vector<dbb::point> v = GetVertices();
    int index[][2] = { // Indices of edge-vertices
      {6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
      {0,1},{0,3},{7,1},{7,4},{4,5},{5,3}
    };
    for (int j = 0; j < 12; ++j)
      result.push_back(dbb::lineSegment(v[index[j][0]], v[index[j][1]]));

    return result;
  }

  //------------------------------------------------------------
  float OBB::PenetrationDepth(const OBB& other, const glm::vec3& axis, bool* outShouldFlip) const
  {
    Interval i1 = GetInterval(*this, glm::normalize(axis));
    Interval i2 = GetInterval(other, glm::normalize(axis));
    if (!((i2.min <= i1.max) && (i1.min <= i2.max)))
      return 0.0f; // No penerattion
    float len1 = i1.max - i1.min;
    float len2 = i2.max - i2.min;
    float min = fminf(i1.min, i2.min);
    float max = fmaxf(i1.max, i2.max);
    float length = max - min;
    if (outShouldFlip != 0)
      *outShouldFlip = (i2.min < i1.min);
    return (len1 + len2) - length;
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
}
