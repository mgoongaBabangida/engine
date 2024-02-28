#include "stdafx.h"

#include "PlaneLine.h"

#include <limits>
#include <iostream>

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

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
	glm::vec3 normal = {plane.A, plane.B, plane.C}; //normalize ?
	float pLen = obb.size.x * fabsf(glm::dot(normal, rot[0])) +
							 obb.size.y * fabsf(glm::dot(normal, rot[1])) +
							 obb.size.z * fabsf(glm::dot(normal, rot[2]));
	float dot = glm::dot(glm::vec3{ plane.A, plane.B, plane.C }, obb.origin); // /normaliz?
	float dist = dot - plane.D;
	return fabsf(dist) <= pLen;
}

//------------------------------------------------------------------
float dbb::Raycast(const dbb::plane& plane, const dbb::ray& ray)
{
	float nd = glm::dot(ray.direction, plane.Normal());
	float pn = glm::dot(ray.origin, plane.Normal());
	if (nd >= 0.0f)
		return -1;

	float t = (plane.D - pn) / nd;

	if (t >= 0.0f)
		return t;
	return -1;
}

bool dbb::LineTest(const dbb::plane& plane, dbb::lineSegment line)
 {
	vec3 ab = line.end - line.start;
	float nA = glm::dot(plane.Normal(), line.start);
	float nAB = glm::dot(plane.Normal(), ab);
	// If the line and plane are parallel, nAB will be 0
	// This will cause a divide by 0 exception below
	// If you plan on testing parallel lines and planes
	// it is sage to early out when nAB is 0. 
	float t = (plane.D - nA) / nAB;
	return t >= 0.0f && t <= 1.0f;
}

//--------------------------------------------------------
glm::vec3 dbb::intersection(dbb::plane P, dbb::line L)
{
	float t = 0.0f;
	if (L.p.x != 0 && L.p.y != 0 && L.p.z != 0)
		t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.x + P.B*L.p.y + P.C*L.p.z);
	else if (L.p.x == 0)
		t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.B*L.p.y + P.C*L.p.z);
	else if (L.p.y == 0)
		t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.x + P.C*L.p.z);
	else if (L.p.z == 0)
		t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.x + P.B*L.p.y);
	else if (L.p.x == 0 && L.p.y == 0) { t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.z); }
	else if (L.p.x == 0 && L.p.z == 0) { t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.y); }
	else if (L.p.y == 0 && L.p.z == 0) { t = -(P.D + P.A*L.M.x + P.B*L.M.y + P.C*L.M.z) / (P.A*L.p.x); }
	else if (L.p.x == 0 && L.p.y == 0 && L.p.z == 0) { std::cout << "t0=" << t << std::endl; /*throw std::exception e;*/ }

	return vec3(L.p.x*t + L.M.x, L.p.y*t + L.M.y, L.p.z*t + L.M.z);
}

/*
A*(p.x*t+M.x)+B(p.y*t+M.y)+C(p.z*t+M.z)+D=0
A*p.x*t+A*M.x+B*p.y*t+B*M.y+C*p.z*t+C*M.z+D=0
A*p.x*t+B*p.y*t+C*p.z*t=-D-A*M.x-B*M.y-C*M.z
t(A*p.x+B*p.y+C*p.z)=-D-A*M.x-B*M.y-C*M.z
t=-(D+A*M.x+B*M.y+C*M.z)/(A*p.x+B*p.y+C*p.z)
*/

/* // else if( L.p.z==0){}
A*(p.x*t+M.x))+B*(p.y*t+M.y))+C*M.z+D=0
A*p.x*t+A*M.x+B*p.y*t+B*M.y+C*M.z+D=0
A*p.x*t+B*p.y*t=-(D+A*M.xB*M.y+C*M.z)
t(A*p.x+B*p.y)=-(D+A*M.xB*M.y+C*M.z)
t=-(D+A*M.x+B*M.y+C*M.z)/(A*p.x+B*p.y)
*/

//--------------------------------------------------------
bool dbb::IsInside(dbb::triangle triangle, dbb::point dot)
{
	glm::vec3 dot1(triangle[0][0], triangle[0][1], triangle[0][2]);
	glm::vec3 dot2(triangle[1][0], triangle[1][1], triangle[1][2]);
	glm::vec3 dot3(triangle[2][0], triangle[2][1], triangle[2][2]);

	dbb::plane tryPlane(triangle);

	if (!tryPlane.isOn(dot))
		return false;

	if (glm::dot(glm::cross(dot2 - dot1, dot3 - dot1), glm::cross(dot2 - dot1, dot - dot1)) <= 0)
		return false;
	if (glm::dot(glm::cross(dot3 - dot2, dot1 - dot2), glm::cross(dot3 - dot2, dot - dot2)) <= 0)
		return false;
	if (glm::dot(glm::cross(dot1 - dot3, dot2 - dot3), glm::cross(dot1 - dot3, dot - dot3)) <= 0)
		return false;

	return true;

}

//Constructors
//--------------------------------------------------------
dbb::plane::plane(float a, float b, float c, float d) :A(a), B(b), C(c), D(d) {}

//--------------------------------------------------------
dbb::plane::plane(dbb::point dot1, dbb::point dot2, dbb::point dot3)
{
	vec3 v1(dot2.x - dot1.x, dot2.y - dot1.y, dot2.z - dot1.z);
	vec3 v2(dot3.x - dot1.x, dot3.y - dot1.y, dot3.z - dot1.z);
	vec3 dot = dot1;
	vec3 normal = cross(v1, v2);
	A = normal.x; B = normal.y; C = normal.z;
	D = -(A*dot.x + B*dot.y + C*dot.z);
}

//--------------------------------------------------------
dbb::plane::plane(dbb::triangle m_dots)
{
	vec3 v1(m_dots[0][0] - m_dots[1][0], m_dots[0][1] - m_dots[1][1], m_dots[0][2] - m_dots[1][2]);
	vec3 v2(m_dots[2][0] - m_dots[1][0], m_dots[2][1] - m_dots[1][1], m_dots[2][2] - m_dots[1][2]);
	vec3 dot(m_dots[1][0], m_dots[1][1], m_dots[1][2]);
	vec3 normal = glm::cross(v1, v2);
	A = normal.x; B = normal.y; C = normal.z;
	D = -(A*dot.x + B*dot.y + C*dot.z);
}

//--------------------------------------------------------
dbb::line::line(dbb::point dot, glm::vec3 direction) :M(dot), p(direction)
{}

//--------------------------------------------------------
bool dbb::line::isOn(dbb::point dot)
{
	float threshhold = 0.001f;

	if (p.x != 0.0f && p.y != 0.0f && p.z != 0.0f)
	{
		if (glm::abs((dot.x - M.x) / p.x - (dot.y - M.y) / p.y) < threshhold && glm::abs((dot.x - M.x) / p.x - (dot.z - M.z) / p.z)< threshhold)
			return true;
		else
			return false;
	}
	else if (p.y == 0 && p.z == 0)
	{
		if (dot.y == M.y && dot.z == M.z)
			return true;
		else
			return false;
	}
	else if (p.x == 0 && p.z == 0)
	{
		if (dot.x == M.x && dot.z == M.z)
			return true;
		else
			return false;
	}
	else if (p.x == 0 && p.y == 0)
	{
		if (dot.x == M.x && dot.y == M.y)
			return true;
		else
			return false;
	}
	else if (p.z == 0)
	{
		if (glm::abs((dot.x - M.x) / p.x - (dot.y - M.y) / p.y) < threshhold && glm::abs(dot.z - M.z)< threshhold)
			return true;
		else
			return false;
	}
	else if (p.y == 0)
	{
		if (glm::abs((dot.x - M.x) / p.x - (dot.z - M.z) / p.z) < threshhold && glm::abs(dot.y - M.y) < threshhold)
			return true;
		else
			return false;
	}
	else if (p.x == 0)
	{
		if (glm::abs((dot.y - M.y) / p.y - (dot.z - M.z) / p.z) < threshhold && glm::abs(dot.x - M.x) < threshhold)
			return true;
		else
			return false;
	}
	else if (p.x == 0 && p.y == 0 && p.z == 0)
	{
		std::cout << "dbb::line::isOn is null" << std::endl;
		return false;
	}
	return false;
}

//--------------------------------------------------------
bool dbb::plane::isOn(dbb::point dot)
{
	float threshhold = 0.001f;
	if (glm::abs(A * dot.x + B * dot.y + C * dot.z + D) < threshhold) // plane equation
		return true;
	else
		return false;
}

//--------------------------------------------------------
bool dbb::plane::isInFront(dbb::point dot)
{
	return A * dot.x + B * dot.y + C * dot.z > D;
}

//--------------------------------------------------------
bool dbb::plane::isSame(dbb::plane other)
{
	if (A / other.A == B / other.B && A / other.A == C / other.C && A / other.A == D / other.D)
		return true;
	else
		return false;

}

//--------------------------------------------------------
float dbb::plane::PlaneEquation(const dbb::point& dot)
{
	return A * dot.x + B * dot.y + C * dot.z - D;
}

//--------------------------------------------------------
dbb::point dbb::plane::GetClosestPointOnPlane(const dbb::point& point) const
{
	float dot = glm::dot(glm::vec3(A,B,C), point);
	float distance = dot - D;
	return point - glm::vec3(A, B, C) * distance;
}

//--------------------------------------------------------
float dbb::line::findT(dbb::point dot) //  isOn parametric
{
	float tx = (dot.x - M.x) / p.x;  // 0 cases
	float ty = (dot.y - M.y) / p.y;
	float tz = (dot.z - M.z) / p.z;

	if (tx == ty && tx == tz)
		return tx;
	else
		return std::numeric_limits<float>::min();
}

//--------------------------------------------------------
glm::vec3 dbb::line::getDotFromT(float t)
{
	glm::vec3 ret;
	ret.x = p.x* t + M.x;
	ret.y = p.y*t + M.y;
	ret.z = p.z*t + M.z;
	return ret;
}
