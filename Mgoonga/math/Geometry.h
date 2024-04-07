#pragma once

#include "math.h"

#include <base/base.h>

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

#include <assert.h>
#include <exception>
#include <vector>

#include "Utils.h"

namespace dbb
{
  using triangle = glm::mat3;
  using point = glm::vec3;

  //returns the barycentric coordinates of a point with respect to a triangle
  glm::vec3 Barycentric(const point& p, const triangle& t);

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
    static AABB FromMinMax(const glm::vec3& min, const glm::vec3& max);
  };

  struct OBB;
  class plane;

  //-------------------------------------------------
  struct lineSegment
  {
    dbb::point start;
    dbb::point end;

    inline lineSegment() {}
    inline lineSegment(const dbb::point& s, const dbb::point& e) :
      start(s), end(e) { }

    float Length() const;
    float LengthSq() const;

    bool LineTest(const dbb::sphere& sphere) const;
    bool LineTest(const AABB& aabb) const;
    bool LineTest(const OBB& obb) const;
    bool LineTest(const dbb::plane& plane);
    bool Linetest(const dbb::triangle& triangle);
  };

  //----------------------------------------------------
  struct DLL_MATH OBB
  {
    dbb::point origin;
    glm::vec3 size;
    glm::mat3 orientation;

    inline OBB() : size(1, 1, 1) { }
    inline OBB(const dbb::point& p, const glm::vec3& s) :
      origin(p), size(s) { }
    inline OBB(const dbb::point& p, const glm::vec3& s, const glm::mat3& o)
      : origin(p), size(s), orientation(o) { }

    std::vector<dbb::point> GetVertices() const;
    std::vector<dbb::lineSegment> GetEdges() const;
    std::vector<dbb::plane> GetPlanes() const;

    float PenetrationDepth(const OBB& other, const glm::vec3& axis, bool* outShouldFlip) const;
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

    static dbb::ray inline FromPoint(const dbb::point& from, const dbb::point& to)
    {
      return ray(from, glm::normalize(to - from));
    }

    float Raycast(const dbb::sphere& sphere, RaycastResult& outResult) const;
    float Raycast(const AABB& aabb, RaycastResult& outResult) const;
    float Raycast(const OBB& obb, RaycastResult& outResult) const;
    float Raycast(const dbb::plane& plane, RaycastResult& outResult) const;
    float Raycast(const dbb::triangle& triangle, RaycastResult& outResult) const;
  };

  //----------------------------------------------------------------------
  class DLL_MATH plane
  {
  public:
    float A;
    float B;
    float C;
    float D;

    plane() {}
    plane(float a, float b, float c, float d);
    plane(dbb::point dot1, dbb::point dot2, dbb::point dot3);
    plane(glm::vec3 normal, float distance) : A(normal.x), B(normal.y), C(normal.z), D(distance) {}
    plane(dbb::triangle _triangle_on_plane);

    glm::vec3 Normal() const { return glm::normalize(glm::vec3{ A, B, C }); }
    bool isOn(dbb::point dot);
    bool isInFront(dbb::point dot);
    bool isSame(dbb::plane other);
    float PlaneEquation(const dbb::point& dot) const;
    dbb::point GetClosestPointOnPlane(const dbb::point& point) const;
  };

  //---------------------------------------------------------------------
  class DLL_MATH line
  {
  public:
    dbb::point M; // dot
    glm::vec3 p; //vector direction

    line(dbb::point dot, glm::vec3 direction);
    line() :M(glm::vec3()), p(glm::vec3()) {}

    bool		isOn(dbb::point dot);
    float		findT(dbb::point dot);
    glm::vec3	getDotFromT(float t);
  };

  DLL_MATH glm::vec3 intersection(dbb::plane P, dbb::line L);

  DLL_MATH bool IsInside(dbb::triangle _triangle, dbb::point _dot);

  //-------------------------------------------------
  struct Interval
  {
    float min;
    float max;
  };

  Interval GetInterval(const AABB& rect, const glm::vec3& axis);
  Interval GetInterval(const OBB& rect, const glm::vec3& axis);
  Interval GetInterval(const dbb::triangle& triangle, const glm::vec3& axis);

  //-------------------------------------------------
  struct CollisionManifold
  {
    bool colliding;
    glm::vec3 normal;
    float depth;
    std::vector<glm::vec3> contacts;
    static void ResetCollisionManifold(CollisionManifold& result);
  };
}