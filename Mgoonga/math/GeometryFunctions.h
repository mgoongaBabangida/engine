#pragma once

#include "math.h"

#include <base/interfaces.h>

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

  //Triangle checks
  bool IsPointInTriangle(const dbb::point& point, const dbb::triangle& triangle);
  dbb::point GetClosestPointOnTriangle(const dbb::triangle& t, const dbb::point& p);

  //Overlap on axis
  bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis);
  bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis);
  bool OverlapOnAxis(const AABB& aabb, const dbb::triangle& triangle, const glm::vec3& axis);
  bool OverlapOnAxis(const OBB& obb, const dbb::triangle& triangle, const glm::vec3& axis);
  bool OverlapOnAxis(const dbb::triangle& t1, const dbb::triangle& t2, const glm::vec3& axis);

  //Collision checks
  bool SphereSphere(const dbb::sphere& s1, const dbb::sphere& s2);
  bool SphereAABB(const dbb::sphere& sphere, const AABB& aabb);
  bool SphereOBB(const dbb::sphere& sphere, const OBB& obb);
  bool AABBAABB(const AABB& aabb1, const AABB& aabb2);
  bool SpherePlane(const dbb::sphere& sphere, const dbb::plane& plane);
  bool AABBPlane(const AABB& aabb, const dbb::plane& plane);
  bool OBBPlane(const OBB& obb, const dbb::plane& plane);
  bool AABBOBB(const AABB& aabb, const OBB& obb);
  bool OBBOBB(const OBB& obb1, const OBB& obb2);

  bool TriangleSphere(const dbb::triangle& t, const dbb::sphere& s);
  bool TriangleAABB(const dbb::triangle& t, const AABB& a);
  bool TriangleOBB(const dbb::triangle& t, const OBB& o);
  bool TrianglePlane(const dbb::triangle& t, const dbb::plane& p);
  bool TriangleTriangle(const dbb::triangle& t1, const dbb::triangle& t2);

  glm::vec3 SatCrossEdge(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);
  bool TriangleTriangleRobust(const dbb::triangle& t1,const dbb::triangle& t2);

  // Mesh checks
  float MeshRay(const I3DMesh& mesh, const dbb::ray& ray);
  bool MeshAABB(const I3DMesh& mesh, const dbb::AABB& aabb);
  bool MeshOBB(const I3DMesh& mesh, const OBB& o);
  bool MeshSphere(const I3DMesh& mesh, const dbb::sphere& s);
  bool MeshPlane(const I3DMesh& mesh, const dbb::plane& p);
  bool MeshTriangle(const I3DMesh& mesh, const dbb::triangle& t2);

  bool ClipToPlane(const dbb::plane& plane, const dbb::lineSegment& line, dbb::point* outPoint);
  std::vector<dbb::point> ClipEdgesToOBB(const std::vector<dbb::lineSegment>& _edges, const OBB& _obb);

  DLL_MATH CollisionManifold FindCollision(const dbb::sphere& A, const dbb::sphere& B);
  DLL_MATH CollisionManifold FindCollision(const dbb::OBB& A, const dbb::sphere& B);
  DLL_MATH CollisionManifold FindCollision(const OBB& A, const OBB& B);
}