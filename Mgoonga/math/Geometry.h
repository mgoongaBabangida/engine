#pragma once

#include "math.h"

#include <base/base.h>

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

#define AABBSphere(aabb, sphere) \
 SphereAABB(Sphere, AABB)

#define OBBSphere(obb, sphere) \
SphereOBB(sphere, obb)

#define OBBAABB(obb, aabb) \
 AABBOBB(aabb, obb)

namespace dbb
{
  using triangle = glm::mat3;
  using point = glm::vec3;

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
  };

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

    std::vector<dbb::point> GetVertices() const;
    std::vector<dbb::lineSegment> GetEdges() const;
    /*std::vector<dbb::plane> GetPlanes();*/ // @todo after getting plane
    /*std::vector<dbb::point> ClipEdgesToOBB(const std::vector<dbb::lineSegment>& edges);*/ // ?
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

    float Raycast(const dbb::sphere& sphere, RaycastResult& outResult);
    float Raycast(const AABB& aabb, RaycastResult& outResult);
    float Raycast(const OBB& obb, RaycastResult& outResult);
  };

  //-------------------------------------------------
  struct Interval
  {
    float min;
    float max;
  };

  Interval GetInterval(const AABB& rect, const glm::vec3& axis);
  Interval GetInterval(const OBB& rect, const glm::vec3& axis);

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