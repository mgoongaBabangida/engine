#include "stdafx.h"

#include "GeometryFunctions.h"
#include "Geometry.h"

#include <cmath>
#include <cfloat>
#include <algorithm>

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

namespace dbb
{
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
      const float* orientation = &obb.orientation[i][0];
      glm::vec3 axis(orientation[0], orientation[1], orientation[2]);
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
      const float* orientation = &obb.orientation[i][0];
      glm::vec3 axis(orientation[0], orientation[1], orientation[2]);
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
  bool IsPointInTriangle(const dbb::point& _point, const dbb::triangle& _triangle)
  {
    dbb::point triangle_p1(_triangle[0][0], _triangle[0][1], _triangle[0][2]);
    dbb::point triangle_p2(_triangle[1][0], _triangle[1][1], _triangle[1][2]);
    dbb::point triangle_p3(_triangle[2][0], _triangle[2][1], _triangle[2][2]);

    glm::vec3 a = triangle_p1 - _point;
    glm::vec3 b = triangle_p2 - _point;
    glm::vec3 c = triangle_p3 - _point;

    // The point should be moved too, so they are both
    // relative, but because we don't use p in the
    // equation anymore, we don't need it!
    // p -= p; This would just equal the zero vector!
    glm::vec3 normPBC = glm::cross(b, c); // Normal of PBC (u)
    glm::vec3 normPCA = glm::cross(c, a); // Normal of PCA (v)
    glm::vec3 normPAB = glm::cross(a, b); // Normal of PAB (w)

    if (glm::dot(normPBC, normPCA) < 0.0f)
      return false;
    else if (glm::dot(normPBC, normPAB) < 0.0f)
      return false;

    return true;
  }

  //-------------------------------------------------------------
  dbb::point GetClosestPointOnTriangle(const dbb::triangle& _triangle, const dbb::point& _point)
  {
    dbb::plane plane(_triangle);
    dbb::point closest = plane.GetClosestPointOnPlane(_point);
    if (IsPointInTriangle(closest, _triangle))
      return closest;

    dbb::point triangle_p1(_triangle[0][0], _triangle[0][1], _triangle[0][2]);
    dbb::point triangle_p2(_triangle[1][0], _triangle[1][1], _triangle[1][2]);
    dbb::point triangle_p3(_triangle[2][0], _triangle[2][1], _triangle[2][2]);

    dbb::point c1 = GetClosestPointOnLineSegment(dbb::lineSegment(triangle_p1, triangle_p2), _point); // Line AB
    dbb::point c2 = GetClosestPointOnLineSegment(dbb::lineSegment(triangle_p2, triangle_p3), _point); // Line BC
    dbb::point c3 = GetClosestPointOnLineSegment(dbb::lineSegment(triangle_p3, triangle_p1), _point); // Line CA

    float magSq1 = glm::length2(_point - c1);
    float magSq2 = glm::length2(_point - c2);
    float magSq3 = glm::length2(_point - c3);

    if (magSq1 < magSq2 && magSq1 < magSq3)
      return c1;

    else if (magSq2 < magSq1 && magSq2 < magSq3)
      return c2;

    return c3;
  }

  //-------------------------------------------------------------
  bool OverlapOnAxis(const AABB& _aabb, const dbb::triangle& _triangle, const glm::vec3& _axis)
  {
    Interval a = GetInterval(_aabb, _axis);
    Interval b = GetInterval(_triangle, _axis);
    return ((b.min <= a.max) && (a.min <= b.max));
  }

  //-------------------------------------------------------------
  bool OverlapOnAxis(const OBB& _obb, const dbb::triangle& _triangle, const glm::vec3& _axis)
  {
    Interval a = GetInterval(_obb, _axis);
    Interval b = GetInterval(_triangle, _axis);
    return ((b.min <= a.max) && (a.min <= b.max));
  }

  //-------------------------------------------------------------
  bool OverlapOnAxis(const dbb::triangle& t1, const dbb::triangle& t2, const glm::vec3& axis)
  {
    Interval a = GetInterval(t1, axis);
    Interval b = GetInterval(t2, axis);
    return ((b.min <= a.max) && (a.min <= b.max));
  }

  //-------------------------------------------------------------
  bool TriangleAABB(const dbb::triangle& _triangle, const AABB& _axis)
  {
    dbb::point triangle_p1(_triangle[0][0], _triangle[0][1], _triangle[0][2]);
    dbb::point triangle_p2(_triangle[1][0], _triangle[1][1], _triangle[1][2]);
    dbb::point triangle_p3(_triangle[2][0], _triangle[2][1], _triangle[2][2]);

    glm::vec3 f0 = triangle_p2 - triangle_p1;
    glm::vec3 f1 = triangle_p3 - triangle_p2;
    glm::vec3 f2 = triangle_p1 - triangle_p3;

    glm::vec3 u0(1.0f, 0.0f, 0.0f);
    glm::vec3 u1(0.0f, 1.0f, 0.0f);
    glm::vec3 u2(0.0f, 0.0f, 1.0f);

    glm::vec3 test[13] = { u0, // AABB Axis 1
                           u1, // AABB Axis 2
                           u2, // AABB Axis 3
      glm::cross(f0, f1),
      glm::cross(u0, f0), glm::cross(u0, f1), glm::cross(u0, f2),
      glm::cross(u1, f0), glm::cross(u1, f1), glm::cross(u1, f2),
      glm::cross(u2, f0), glm::cross(u2, f1), glm::cross(u2, f2)
    };

    for (int i = 0; i < 13; ++i)
    {
      if (!OverlapOnAxis(_axis, _triangle, test[i]))
        return false; // Separating axis found
    }
    return true; // Separating axis not found
  }

  //-------------------------------------------------------------
  bool TriangleOBB(const dbb::triangle& _triangle, const OBB& _obb)
  {
    dbb::point triangle_p1(_triangle[0][0], _triangle[0][1], _triangle[0][2]);
    dbb::point triangle_p2(_triangle[1][0], _triangle[1][1], _triangle[1][2]);
    dbb::point triangle_p3(_triangle[2][0], _triangle[2][1], _triangle[2][2]);

    // Compute the edge vectors of the triangle (ABC)
    glm::vec3 f0 = triangle_p2 - triangle_p1;
    glm::vec3 f1 = triangle_p3 - triangle_p2;
    glm::vec3 f2 = triangle_p1 - triangle_p3;

    const float* orientation = &_obb.orientation[0][0];
    glm::vec3 u0(orientation[0],
                 orientation[1],
                 orientation[2]);
    glm::vec3 u1(orientation[3],
                 orientation[4],
                 orientation[5]);
    glm::vec3 u2(orientation[6],
                 orientation[7],
                 orientation[8]);

    glm::vec3 test[13] = {
      u0, // OBB Axis 1
      u1, // OBB Axis 2
      u2, // OBB Axis 3
      glm::cross(f0, f1), // Normal of the Triangle 
      glm::cross(u0, f0), glm::cross(u0, f1), glm::cross(u0, f2),
      glm::cross(u1, f0), glm::cross(u1, f1), glm::cross(u1, f2),
      glm::cross(u2, f0), glm::cross(u2, f1), glm::cross(u2, f2)
    };

    for (int i = 0; i < 13; ++i)
    {
      if (!OverlapOnAxis(_obb, _triangle, test[i]))
        return false; // Separating axis found
    }
    return true; // Separating axis not found
  }

  //-------------------------------------------------------------
  bool TrianglePlane(const dbb::triangle& _triangle, const dbb::plane& _plane)
  {
    float side1 = _plane.PlaneEquation({_triangle[0][0], _triangle[0][1], _triangle[0][2]});
    float side2 = _plane.PlaneEquation({_triangle[0][0], _triangle[0][1], _triangle[0][2]});
    float side3 = _plane.PlaneEquation({_triangle[0][0], _triangle[0][1], _triangle[0][2]});

    if (side1 == 0 && side2 == 0 && side3 == 0)
      return true;

    if (side1 > 0 && side2 > 0 && side3 > 0)
      return false;

    if (side1 < 0 && side2 < 0 && side3 < 0)
      return false;

    return true; // Intersection
  }

  //-------------------------------------------------------------
  bool TriangleTriangle(const dbb::triangle& _t1, const dbb::triangle& _t2)
  {
    dbb::point triangle1_p1(_t1[0][0], _t1[0][1], _t1[0][2]);
    dbb::point triangle1_p2(_t1[1][0], _t1[1][1], _t1[1][2]);
    dbb::point triangle1_p3(_t1[2][0], _t1[2][1], _t1[2][2]);

    dbb::point triangle2_p1(_t2[0][0], _t2[0][1], _t2[0][2]);
    dbb::point triangle2_p2(_t2[1][0], _t2[1][1], _t2[1][2]);
    dbb::point triangle2_p3(_t2[2][0], _t2[2][1], _t2[2][2]);

    glm::vec3 t1_f0 = triangle1_p2 - triangle1_p1; // Triangle 1, Edge 0
    glm::vec3 t1_f1 = triangle1_p3 - triangle1_p2; // Triangle 1, Edge 1
    glm::vec3 t1_f2 = triangle1_p1 - triangle1_p3; // Triangle 1, Edge 

    glm::vec3 t2_f0 = triangle2_p2 - triangle2_p1; // Triangle 2, Edge 0
    glm::vec3 t2_f1 = triangle2_p3 - triangle2_p2; // Triangle 2, Edge 1
    glm::vec3 t2_f2 = triangle2_p1 - triangle2_p3; // Triangle 2, Edge 2

    glm::vec3 axisToTest[] = {
      glm::cross(t1_f0, t1_f1),
      glm::cross(t2_f0, t2_f1),
      glm::cross(t2_f0, t1_f0), glm::cross(t2_f0, t1_f1),
      glm::cross(t2_f0, t1_f2), glm::cross(t2_f1, t1_f0),
      glm::cross(t2_f1, t1_f1), glm::cross(t2_f1, t1_f2),
      glm::cross(t2_f2, t1_f0), glm::cross(t2_f2, t1_f1),
      glm::cross(t2_f2, t1_f2)};

    for (int i = 0; i < 11; ++i)
    {
      if (!OverlapOnAxis(_t1, _t2, axisToTest[i]))
        return false; // Seperating axis found
    }
    return true; // Seperating axis not found
  }

  //-------------------------------------------------------------
  glm::vec3 SatCrossEdge(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
  {
    glm::vec3 ab = a - b;
    glm::vec3 cd = c - d;
    glm::vec3 result = glm::cross(ab, cd);

    if (!glm::length2(result) == 0)
      return result; // Not parallel!
    else
    {
      glm::vec3 axis = glm::cross(ab, c - a);
      result = glm::cross(ab, axis);
      if (!glm::length2(result) == 0)
        return result; // Not parallel
    }
    return glm::vec3();
  }

  //-------------------------------------------------------------
  bool TriangleTriangleRobust(const dbb::triangle& _t1, const dbb::triangle& _t2)
  {
    dbb::point triangle1_p1(_t1[0][0], _t1[0][1], _t1[0][2]);
    dbb::point triangle1_p2(_t1[1][0], _t1[1][1], _t1[1][2]);
    dbb::point triangle1_p3(_t1[2][0], _t1[2][1], _t1[2][2]);

    dbb::point triangle2_p1(_t2[0][0], _t2[0][1], _t2[0][2]);
    dbb::point triangle2_p2(_t2[1][0], _t2[1][1], _t2[1][2]);
    dbb::point triangle2_p3(_t2[2][0], _t2[2][1], _t2[2][2]);

    glm::vec3 axisToTest[] = {
      // Triangle 1, Normal
      SatCrossEdge(triangle1_p1, triangle1_p2, triangle1_p2, triangle1_p3),
      // Triangle 2, Normal
      SatCrossEdge(triangle2_p1, triangle2_p2, triangle2_p2, triangle2_p3),
      SatCrossEdge(triangle2_p1, triangle2_p2, triangle1_p1, triangle1_p2),
      SatCrossEdge(triangle2_p1, triangle2_p2, triangle1_p2, triangle1_p3),
      SatCrossEdge(triangle2_p1, triangle2_p2, triangle1_p3, triangle1_p1),
      SatCrossEdge(triangle2_p2, triangle2_p3, triangle1_p1, triangle1_p2),
      SatCrossEdge(triangle2_p2, triangle2_p3, triangle1_p2, triangle1_p3),
      SatCrossEdge(triangle2_p2, triangle2_p3, triangle1_p3, triangle1_p1),
      SatCrossEdge(triangle2_p3, triangle2_p1, triangle1_p1, triangle1_p2),
      SatCrossEdge(triangle2_p3, triangle2_p1, triangle1_p2, triangle1_p3),
      SatCrossEdge(triangle2_p3, triangle2_p1, triangle1_p3, triangle1_p1),
    };

    for (int i = 0; i < 11; ++i) {
      if (!OverlapOnAxis(_t1, _t2, axisToTest[i]))
      {
        if (!glm::length2(axisToTest[i]) == 0)
          return false; // Seperating axis found
      }
    }
    return true; // Seperating axis not found
  }

  //-------------------------------------------------------------
  float MeshRay(const I3DMesh& _mesh, const dbb::ray& _ray)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i+=3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i+1]].Position}, {vertices[indices[i+2]].Position} };
      RaycastResult res;
      float result = _ray.Raycast(tr, res);
      if (result >= 0) {
        return result;
      }
    }
    return false;
  }

  //-------------------------------------------------------------
  bool MeshAABB(const I3DMesh& _mesh, const dbb::AABB& aabb)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i += 3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i + 1]].Position}, {vertices[indices[i + 2]].Position} };
      if (TriangleAABB(tr, aabb))
        return true;
    }
    return false;
  }

  //-------------------------------------------------------------
  bool MeshOBB(const I3DMesh& _mesh, const OBB& _obb)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i += 3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i + 1]].Position}, {vertices[indices[i + 2]].Position} };
      if (TriangleOBB(tr, _obb))
        return true;
    }
    return false;
  }

  //-------------------------------------------------------------
  bool MeshSphere(const I3DMesh& _mesh, const dbb::sphere& _s)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i += 3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i + 1]].Position}, {vertices[indices[i + 2]].Position} };
      if (TriangleSphere(tr, _s))
        return true;
    }
    return false;
  }

  //-------------------------------------------------------------
  bool MeshPlane(const I3DMesh& _mesh, const dbb::plane& _p)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i += 3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i + 1]].Position}, {vertices[indices[i + 2]].Position} };
      if (TrianglePlane(tr, _p))
        return true;
    }
    return false;
  }

  //-------------------------------------------------------------
  bool MeshTriangle(const I3DMesh& _mesh, const dbb::triangle& _t)
  {
    size_t points_count = _mesh.GetIndices().size();
    std::vector<Vertex> vertices = _mesh.GetVertexs(); //@todo avoid copy, get ref
    std::vector<unsigned int> indices = _mesh.GetIndices();
    for (int i = 0; i < points_count; i += 3)
    {
      dbb::triangle tr = { {vertices[indices[i]].Position}, {vertices[indices[i + 1]].Position}, {vertices[indices[i + 2]].Position} };
      if (TriangleTriangle(tr, _t))
        return true;
    }
    return false;
  }

  //-------------------------------------------------------------
  bool SphereSphere(const dbb::sphere& s1, const dbb::sphere& s2)
  {
    float radiiSum = s1.radius + s2.radius;
    float sqDistance = glm::length2(s1.position - s2.position);
    return sqDistance < radiiSum* radiiSum;
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

  //---------------------------------------------------------------------
  bool dbb::SpherePlane(const dbb::sphere& sphere, const dbb::plane& plane)
  {
    dbb::point closestPoint = plane.GetClosestPointOnPlane(sphere.position);
    float distSq = glm::length2(sphere.position - closestPoint);
    float radiusSq = sphere.radius * sphere.radius;
    return distSq < radiusSq;
  }

  //--------------------------------------------------------
  bool dbb::AABBPlane(const AABB& aabb, const dbb::plane& plane)
  {
    float pLen = aabb.size.x * fabsf(plane.A) +
      aabb.size.y * fabsf(plane.B) +
      aabb.size.z * fabsf(plane.C);
    float dot = glm::dot({ plane.A, plane.B, plane.B }, aabb.origin);
    float dist = dot - plane.D;
    return fabsf(dist) <= pLen;
  }

  //--------------------------------------------------------
  bool dbb::OBBPlane(const OBB& obb, const dbb::plane& plane)
  {
    // Local variables for readability only
    const float* o = &obb.orientation[0][0];
    glm::vec3 rot[] = { // rotation / orientation
    glm::vec3(o[0], o[1], o[2]),
    glm::vec3(o[3], o[4], o[5]),
    glm::vec3(o[6], o[7], o[8]),
    };
    glm::vec3 normal = { plane.A, plane.B, plane.C }; //normalize ?
    float pLen = obb.size.x * fabsf(glm::dot(normal, rot[0])) +
      obb.size.y * fabsf(glm::dot(normal, rot[1])) +
      obb.size.z * fabsf(glm::dot(normal, rot[2]));
    float dot = glm::dot(glm::vec3{ plane.A, plane.B, plane.C }, obb.origin); // /normaliz?
    float dist = dot - plane.D;
    return fabsf(dist) <= pLen;
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

  //---------------------------------------------------------------------------------
  bool EllipseOBB(const ellipse& ellipse, const OBB& obb)
  {
    // Transform the ellipse center into the coordinate system of the OBB
      glm::vec3 centerInOBB = glm::inverse(obb.orientation) * (ellipse.center - obb.origin);

    // Transform the ellipse orientation to the coordinate system of the OBB
    glm::mat3 orientationInOBB = glm::transpose(glm::inverse(obb.orientation)) * ellipse.orientation;

    std::vector<glm::vec3> vertices = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    };

    // Transform vertices by the orientation matrix
    for (glm::vec3& vertex : vertices)
      vertex = obb.orientation * vertex;


    // Calculate half extents by finding the maximum distance from the origin along each axis
    glm::vec3 halfExtents;
    for (int i = 0; i < 3; ++i)
    {
      auto minmax = std::minmax_element(vertices.begin(), vertices.end(), [&](const glm::vec3& a, const glm::vec3& b) { return a[i] < b[i]; });
      float maxDistance = std::max(std::abs((*minmax.second)[i]), std::abs((*minmax.first)[i]));
      halfExtents[i] = maxDistance;
    }

    // Calculate the squared distance from the ellipse center to the OBB
    float squaredDistance = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
      float distanceAlongAxis = glm::clamp(centerInOBB[i], -halfExtents[i], halfExtents[i]);
      squaredDistance += glm::pow(distanceAlongAxis - centerInOBB[i], 2.0f);
    }

    // Check if the squared distance is less than the squared radii of the ellipse
    bool intersects = (squaredDistance < glm::length2(ellipse.radii));

    // Additional check for orientation
    // Calculate the squared distance from the ellipse center to the OBB based on orientation
    // This check ensures that the ellipse doesn't intersect the OBB if it's rotated in such a way that it lies completely outside
    // of the OBB's bounding box in any orientation
    if (intersects)
    {
      glm::mat3 rotationMatrix = glm::mat3(1.0f);
      for (int i = 0; i < 3; ++i)
      {
        float distance = centerInOBB[i];
        rotationMatrix[i] *= distance;
      }

      glm::vec3 rotatedRadii = glm::mat3{ glm::abs(orientationInOBB[0]), glm::abs(orientationInOBB[1]), glm::abs(orientationInOBB[2]) } *ellipse.radii;
      float squaredDistanceWithOrientation = glm::length2(rotationMatrix * rotatedRadii);
      intersects = (squaredDistanceWithOrientation < 1.0f);
    }
    return intersects;
  }

  //---------------------------------------------------------------------------------
  bool EllipseSphere(const dbb::ellipse& _ellipse, const dbb::sphere& _sphere)
  {
    // Transform the center of the sphere to the coordinate system of the ellipse
    glm::vec3 sphereCenterInEllipseSpace = glm::inverse(_ellipse.orientation) * (_sphere.position - _ellipse.center);

    // Calculate the distance between the center of the ellipse and the center of the sphere in the ellipse's coordinate system
    float distance = glm::length(sphereCenterInEllipseSpace);

    // Calculate the radius of the sphere projected onto the ellipse's coordinate system
    float sphereRadiusInEllipseSpace = glm::max(_sphere.radius, glm::length(glm::inverse(_ellipse.orientation) * glm::vec3(_sphere.radius)));

    // Check if the distance between the centers of the ellipse and the sphere is less than the sum of their radii
    return distance <= sphereRadiusInEllipseSpace + glm::length(_ellipse.radii);
  }

  //---------------------------------------------------------------------------------
  bool EllipseEllipse(const dbb::ellipse& _ellipse1, const dbb::ellipse& _ellipse2)
  {
    // Transform the center of ellipse2 to the coordinate system of ellipse1
    glm::vec3 center2InEllipse1Space = glm::inverse(_ellipse1.orientation) * (_ellipse2.center - _ellipse1.center);

    // Calculate the radii of ellipse2 projected onto the coordinate system of ellipse1
    glm::vec3 radii2InEllipse1Space = glm::abs(glm::inverse(_ellipse1.orientation) * _ellipse2.orientation * _ellipse2.radii); //abs?

    // Calculate the distance between the centers of the two ellipses in the coordinate system of ellipse1
    float distance = glm::length(center2InEllipse1Space);

    // Check if the distance between the centers of the ellipses is less than the sum of their radii
    return distance <= glm::length(_ellipse1.radii + radii2InEllipse1Space);
  }

  //---------------------------------------------------------------------------------
  bool TriangleSphere(const dbb::triangle& _triangle, const dbb::sphere& _sphere)
  {
    dbb::point closest = GetClosestPointOnTriangle(_triangle, _sphere.position);
    float magSq = glm::length2(closest - _sphere.position);
    return magSq <= _sphere.radius * _sphere.radius;
  }

  //---------------------------------------------------------------------------------
  bool ClipToPlane(const dbb::plane& plane, const dbb::lineSegment& line, dbb::point* outPoint)
  {
    glm::vec3 ab = line.end - line.start;
    float nAB = glm::dot(plane.Normal(), ab);
    if (nAB == 0)
      return false;
    float nA = glm::dot(plane.Normal(), line.start);
    float t = (plane.D - nA) / nAB;
    if (t >= 0.0f && t <= 1.0f) {
      if (outPoint != 0)
        *outPoint = line.start + ab * t;
      return true;
    }
    return false;
  }

  //------------------------------------------------------------
  std::vector<dbb::point> ClipEdgesToOBB(const std::vector<dbb::lineSegment>& _edges, const OBB& _obb)
  {
    std::vector<dbb::point> result;
    result.reserve(_edges.size());
    dbb::point intersection;
    std::vector<dbb::plane> planes = _obb.GetPlanes();
    for (int i = 0; i < planes.size(); ++i)
    {
      for (int j = 0; j < _edges.size(); ++j)
      {
        if (ClipToPlane(planes[i], _edges[j], &intersection))
          if (IsPointInOBB(intersection, _obb))
            result.push_back(intersection);
      }
    }
    return result;
  }

  //---------------------------------------------------------------------------------
  CollisionManifold FindCollision(const dbb::sphere& A, const dbb::sphere& B)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);
    float r = A.radius + B.radius;
    glm::vec3 d = B.position - A.position;
    if (glm::length2(d) - r * r > 0 || glm::length2(d) == 0.0f)
      return result;
    glm::normalize(d);
    result.colliding = true;
    result.normal = d;
    result.depth = fabsf(glm::length(d) - r) * 0.5f;
    // dtp - Distance to intersection point
    float dtp = A.radius - result.depth;
    dbb::point contact = A.position + d * dtp;
    result.contacts.push_back(contact);
    return result;
  }

  //---------------------------------------------------------------------------------
  CollisionManifold FindCollision(const dbb::OBB& A, const dbb::sphere& B)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);
    dbb::point closestPoint = GetClosestPointOnOBB(A, B.position);
    float distanceSq = glm::length2(closestPoint - B.position);
    if (distanceSq > B.radius * B.radius)
      return result;

    glm::vec3 normal;
    if (distanceSq == 0.0f)
    {
      float mSq = glm::length2(closestPoint - A.origin);
      if (mSq == 0.0f)
        return result;
      // Closest point is at the center of the sphere
      normal = glm::normalize(closestPoint - A.origin);
    }
    else
      normal = glm::normalize(B.position - closestPoint);

    dbb::point outsidePoint = B.position - normal * B.radius;
    float distance = glm::length(closestPoint - outsidePoint);

    result.colliding = true;
    result.contacts.push_back(closestPoint + (outsidePoint - closestPoint) * 0.5f);
    result.normal = normal;
    result.depth = distance * 0.5f;

    return result;
  }

  //---------------------------------------------------------------------------------
  CollisionManifold FindCollision(const OBB& A, const OBB& B)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);

    const float* o1 = &A.orientation[0][0];
    const float* o2 = &B.orientation[0][0];
    glm::vec3 test[15] = { // Face axis
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

    glm::vec3* hitNormal = 0;
    bool shouldFlip;
    for (int i = 0; i < 15; ++i)
    {
      if (glm::length2(test[i]) < 0.001f)
        continue;

      float depth = A.PenetrationDepth(B, test[i], &shouldFlip);
      if (depth <= 0.0f)
        return result;
      else if (depth < result.depth)
      {
        if (shouldFlip)
          test[i] = test[i] * -1.0f;
        result.depth = depth;
        hitNormal = &test[i];
      }
    }
    if (hitNormal == 0)
      return result;

    glm::vec3 axis = normalize(*hitNormal);
    std::vector<dbb::point> c1 = ClipEdgesToOBB(B.GetEdges(), A);
    std::vector<dbb::point> c2 = ClipEdgesToOBB(A.GetEdges(), B);

    result.contacts.reserve(c1.size() + c2.size());
    result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
    result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

    Interval i = GetInterval(A, axis);
    float distance = (i.max - i.min) * 0.5f - result.depth * 0.5f;
    glm::vec3 pointOnPlane = A.origin + axis * distance;

    for (int i = result.contacts.size() - 1; i >= 0; --i)
    {
      glm::vec3 contact = result.contacts[i];
      result.contacts[i] = contact + (axis * glm::dot(axis, pointOnPlane - contact));

      for (int j = result.contacts.size() - 1; j > i; --j)
      {
        if (glm::length2(result.contacts[j] - result.contacts[i]) < 0.0001f)
        {
          result.contacts.erase(result.contacts.begin() + j);
          break;
        }
      }
    }

    result.colliding = true;
    result.normal = axis;
    return result;
  }

  //-----------------------------------------------------------------------------------
  CollisionManifold FindCollision(const dbb::sphere& sphere, const dbb::ellipse& ellipse)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);

    // Transform the center of the sphere to the coordinate system of the ellipse
    glm::vec3 centerInEllipseSpace = glm::inverse(ellipse.orientation) * (sphere.position - ellipse.center);

    // Calculate the closest point on the ellipse to the center of the sphere
    glm::vec3 closestPointOnEllipse = glm::clamp(centerInEllipseSpace, -ellipse.radii, ellipse.radii);

    // Calculate the vector from the closest point on the ellipse to the center of the sphere
    glm::vec3 closestToPointVector = centerInEllipseSpace - closestPointOnEllipse;

    // Check if the sphere intersects the ellipse
    if (glm::length(closestToPointVector) <= sphere.radius)
    {
      // Calculate the penetration depth
      result.depth = sphere.radius - glm::length(closestToPointVector);

      // Calculate the intersection point on the ellipse
      glm::vec3 intersectionPoint = ellipse.center + ellipse.orientation * closestPointOnEllipse;

      // Add the intersection point to the set of intersection points
      result.contacts.push_back(intersectionPoint);
    }

    // Calculate the collision normal based on the closest point on the ellipse
    result.normal = glm::normalize(ellipse.center + ellipse.orientation * closestPointOnEllipse - sphere.position);

    // Check for intersection
    result.colliding = (glm::length(closestToPointVector) <= sphere.radius);
    return result;
  }

  //-----------------------------------------------------------------------------------
  CollisionManifold FindCollision(const OBB& obb, const dbb::ellipse& ellipse)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);

    // Transform the center of the ellipse to the coordinate system of the OBB
    glm::vec3 centerInOBB = glm::inverse(obb.orientation) * (ellipse.center - obb.origin);

    // Calculate the closest point on the OBB to the center of the ellipse
    glm::vec3 closestPointOnOBB = glm::clamp(centerInOBB, -obb.size, obb.size);
    closestPointOnOBB = obb.orientation * closestPointOnOBB + obb.origin;

    // Calculate the vector from the closest point on the OBB to the center of the ellipse
    glm::vec3 closestToPointVector = ellipse.center - closestPointOnOBB;

    // Check if the ellipse intersects the OBB's faces
    for (int i = 0; i < 3; ++i)
    {
      glm::vec3 axis = glm::normalize(obb.orientation[i]);

      // Calculate the projected radius of the ellipse along the current axis
      float ellipseRadiusAlongAxis = glm::length(ellipse.orientation[i]) * ellipse.radii[i];

      // Calculate the penetration depth along the current axis
      float penetrationDepth = glm::dot(closestToPointVector, axis);

      // Check if the ellipse intersects the face along the current axis
      if (glm::abs(penetrationDepth) < ellipseRadiusAlongAxis)
      {
        // Calculate the intersection point on the face
        glm::vec3 intersectionPoint = closestPointOnOBB + axis * penetrationDepth;
        result.contacts.push_back(intersectionPoint);
        result.colliding = true;
      }
    }

    // Check if the ellipse intersects the edges of the OBB
    for (int i = 0; i < 3; ++i) {
      glm::vec3 axis1 = glm::normalize(obb.orientation[i]);
      for (int j = i + 1; j < 3; ++j) {
        glm::vec3 axis2 = glm::normalize(obb.orientation[j]);
        glm::vec3 edgeDirection = glm::cross(axis1, axis2);

        // Calculate the projected radius of the ellipse along the edge direction
        float ellipseRadiusAlongEdge = glm::length(ellipse.orientation * glm::cross(axis1, axis2)) * glm::length(ellipse.radii);

        // Calculate the penetration depth along the edge direction
        float penetrationDepth = glm::dot(closestToPointVector, edgeDirection);

        // Check if the ellipse intersects the edge
        if (glm::abs(penetrationDepth) < ellipseRadiusAlongEdge)
        {
          // Calculate the intersection point on the edge
          glm::vec3 intersectionPoint = closestPointOnOBB + edgeDirection * penetrationDepth;
          result.contacts.push_back(intersectionPoint);
          result.colliding = true;
        }
      }
    }

    // Check if the ellipse intersects the corners of the OBB
    for (float sign1 : {-1.0f, 1.0f}) {
      for (float sign2 : {-1.0f, 1.0f}) {
        for (float sign3 : {-1.0f, 1.0f}) {
          glm::vec3 corner = obb.origin + sign1 * obb.size[0] * obb.orientation[0] +
            sign2 * obb.size[1] * obb.orientation[1] +
            sign3 * obb.size[2] * obb.orientation[2];

          // Calculate the vector from the corner to the center of the ellipse
          glm::vec3 cornerToCenterVector = ellipse.center - corner;

          // Calculate the squared distance from the corner to the center of the ellipse
          float squaredDistance = glm::dot(cornerToCenterVector, cornerToCenterVector);

          // Check if the squared distance is less than the squared radii of the ellipse
          if (squaredDistance < glm::dot(ellipse.radii, ellipse.radii))
          {
            // Calculate the intersection point on the corner
            result.contacts.push_back(corner);
            result.colliding = true;
          }
        }
      }
    }

    // Calculate the collision normal and penetration depth based on the closest point on the OBB
    result.normal = glm::normalize(ellipse.center - closestPointOnOBB);
    result.depth = glm::length(ellipse.center - closestPointOnOBB) - glm::length(ellipse.radii);

    // Check for intersection
    bool intersects = (result.depth <= 0);

    return result;
  }

  //-----------------------------------------------------------------------------------
  CollisionManifold FindCollision(const dbb::ellipse& ellipse1, const dbb::ellipse& ellipse2)
  {
    CollisionManifold result;
    CollisionManifold::ResetCollisionManifold(result);

    // Transform the center of ellipse2 to the coordinate system of ellipse1
    glm::vec3 centerInEllipse1Space = glm::inverse(ellipse1.orientation) * (ellipse2.center - ellipse1.center);

    // Calculate the squared distances from the center of ellipse2 to the axes of ellipse1
    glm::vec3 distances = glm::pow(centerInEllipse1Space / ellipse1.radii, glm::vec3(2.0f));

    // Check if the center of ellipse2 is inside ellipse1
    if (glm::length(distances) < 1.0f) {
      // Calculate the intersection points in the coordinate system of ellipse1
      glm::vec3 centerToIntersection = glm::sqrt(1.0f - distances) * ellipse1.radii;

      // Transform the intersection points back to the original coordinate system
      glm::mat3 inverseOrientation = glm::transpose(ellipse1.orientation);
      for (float x : {-1.0f, 1.0f}) {
        for (float y : {-1.0f, 1.0f}) {
          for (float z : {-1.0f, 1.0f}) {
            glm::vec3 intersectionPoint = ellipse1.center + inverseOrientation * (centerToIntersection * glm::vec3(x, y, z));
            result.contacts.push_back(intersectionPoint);
            result.colliding = true;
          }
        }
      }

      // Calculate the collision normal based on the center of ellipse2
      result.normal = glm::normalize(ellipse2.center - ellipse1.center);

      // Calculate the penetration depth
      result.depth = glm::length(centerInEllipse1Space) - glm::length(ellipse2.center - ellipse1.center);

      return result;
    }
  }
}