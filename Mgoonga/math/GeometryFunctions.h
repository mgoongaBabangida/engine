#pragma once

#include "math.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

#include <vector>
#include <assert.h>
#include <exception>

#include "Utils.h"

namespace dbb
{
  using triangle = glm::mat3;
  using point = glm::vec3;

  struct sphere;
  struct AABB;
  struct lineSegment;
  struct OBB;
  struct ray;
  class plane;
  class line;
  struct CollisionManifold;

  //Point checks
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

  //Collision checks
  bool SphereSphere(const dbb::sphere& s1, const dbb::sphere& s2);
  bool SphereAABB(const dbb::sphere& sphere, const AABB& aabb);
  bool SphereOBB(const dbb::sphere& sphere, const OBB& obb);
  bool AABBAABB(const AABB& aabb1, const AABB& aabb2);
  bool SpherePlane(const dbb::sphere& sphere, const dbb::plane& plane);
  bool AABBPlane(const AABB& aabb, const dbb::plane& plane);
  bool OBBPlane(const OBB& obb, const dbb::plane& plane);

  bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis);
  bool AABBOBB(const AABB& aabb, const OBB& obb);
  bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis);
  bool OBBOBB(const OBB& obb1, const OBB& obb2);

  bool ClipToPlane(const dbb::plane& plane, const dbb::lineSegment& line, dbb::point* outPoint);
  std::vector<dbb::point> ClipEdgesToOBB(const std::vector<dbb::lineSegment>& _edges, const OBB& _obb);

  CollisionManifold FindCollision(const dbb::sphere& A, const dbb::sphere& B);
  CollisionManifold FindCollision(const dbb::OBB& A, const dbb::sphere& B);
  CollisionManifold FindCollision(const OBB& A, const OBB& B);
}