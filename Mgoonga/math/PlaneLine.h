#pragma once

#include "math.h"

#include "Utils.h"
#include "Geometry.h"

#include <assert.h>
#include <exception>
#include <vector>

using glm::vec3;
using glm::vec4;
using glm::mat4;

#define PlaneSphere(plane, sphere) \
 SpherePlane(sphere, plane)

#define PlaneOBB(plane, obb) \
 OBBPlane(obb, plane)

namespace dbb
{
	//----------------------------------------------------------------------
	class DLL_MATH plane
	{
	public:
		float A = std::numeric_limits<float>::min();
		float B	= std::numeric_limits<float>::min();
		float C	= std::numeric_limits<float>::min();
		float D	= std::numeric_limits<float>::min();

		plane() {}
		plane(float a, float b, float c, float d);
		plane(dbb::point dot1, dbb::point dot2, dbb::point dot3);
		plane(glm::vec3 normal, float distance) : A(normal.x), B(normal.y), C(normal.z), D(distance) {}
		plane(dbb::triangle _triangle_on_plane);
		
		glm::vec3 Normal() const { return glm::normalize(glm::vec3{ A, B, C }); }
		bool isOn(dbb::point dot);
		bool isInFront(dbb::point dot);
		bool isSame(dbb::plane other);
		float PlaneEquation(const dbb::point& dot);
		dbb::point GetClosestPointOnPlane(const dbb::point& point) const;
	};

	float Raycast(const dbb::plane& plane, const dbb::ray& ray, RaycastResult& outResult); //@todo to transfer later
	bool LineTest(const dbb::plane& plane, dbb::lineSegment);
	//bool Raycast(const dbb::triangle& triangle, const dbb::ray& ray, RaycastResult& outResult); // @todo to implement
	std::vector<dbb::plane> GetPlanes(const dbb::OBB& _obb);

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
}

//http://trivial-programming.blogspot.de/2009/04/v1.html

