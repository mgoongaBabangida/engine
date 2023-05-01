#include "stdafx.h"
#include "PlaneLine.h"

#include <iostream>

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
	else if (L.p.x == 0 && L.p.y == 0 && L.p.z == 0) { std::cout << "t0=" << t << std::endl; /*throw std::exception e;*/ } //!

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



bool dbb::IsInside(glm::mat3 triangle, glm::vec3 dot)
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
dbb::plane::plane(float a, float b, float c, float d) :A(a), B(b), C(c), D(d) {}

dbb::plane::plane(glm::vec3 dot1, glm::vec3 dot2, glm::vec3 dot3)
{
	vec3 v1(dot2.x - dot1.x, dot2.y - dot1.y, dot2.z - dot1.z);
	vec3 v2(dot3.x - dot1.x, dot3.y - dot1.y, dot3.z - dot1.z);
	vec3 dot = dot1;
	vec3 normal = cross(v1, v2);
	A = normal.x; B = normal.y; C = normal.z;
	D = -(A*dot.x + B*dot.y + C*dot.z);

}

dbb::plane::plane(glm::mat3 m_dots)
{
	vec3 v1(m_dots[0][0] - m_dots[1][0], m_dots[0][1] - m_dots[1][1], m_dots[0][2] - m_dots[1][2]);
	vec3 v2(m_dots[2][0] - m_dots[1][0], m_dots[2][1] - m_dots[1][1], m_dots[2][2] - m_dots[1][2]);
	vec3 dot(m_dots[1][0], m_dots[1][1], m_dots[1][2]);
	vec3 normal = glm::cross(v1, v2);
	A = normal.x; B = normal.y; C = normal.z;
	D = -(A*dot.x + B*dot.y + C*dot.z);
}

dbb::line::line(glm::vec3 dot, glm::vec3 direction) :M(dot), p(direction)
{

}



bool dbb::line::isOn(glm::vec3 dot)
{
	float threshhold = 0.001f;

	if (p.x != 0.0f && p.y != 0.0f && p.z != 0.0f) {
		if (glm::abs((dot.x - M.x) / p.x - (dot.y - M.y) / p.y)< 0.001f  && glm::abs((dot.x - M.x) / p.x - (dot.z - M.z) / p.z)<0.001f)
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
		if (glm::abs((dot.x - M.x) / p.x - (dot.y - M.y) / p.y) < 0.001f && glm::abs(dot.z - M.z)< 0.001f) //?
			return true;
		else
			return false;
	}
	else if (p.y == 0)
	{
		if (glm::abs((dot.x - M.x) / p.x - (dot.z - M.z) / p.z) < 0.001f && glm::abs(dot.y - M.y) < 0.001f) //?
			return true;
		else
			return false;
	}
	else if (p.x == 0)
	{
		if (glm::abs((dot.y - M.y) / p.y - (dot.z - M.z) / p.z) < 0.001f  && glm::abs(dot.x - M.x) < 0.001f)
			return true;
		else
			return false;
	}
	else if (p.x == 0 && p.y == 0 && p.z == 0)
	{
		std::cout << "null" << std::endl;
		return false;
	}
	return false;
}

bool dbb::plane::isOn(glm::vec3 dot)
{
	if (glm::abs(A * dot.x + B * dot.y + C * dot.z + D)<0.001f)
		return true;
	else
		return false;

}

bool dbb::plane::isSame(plane other)
{
	if (A / other.A == B / other.B && A / other.A == C / other.C && A / other.A == D / other.D)
		return true;
	else
		return false;

}

float dbb::line::findT(glm::vec3 dot) //  isOn parametric
{
	float tx = (dot.x - M.x) / p.x;  // 0 cases
	float ty = (dot.y - M.y) / p.y;
	float tz = (dot.z - M.z) / p.z;

	if (tx == ty && tx == tz)
		return tx;
	else
		return -10000;  // ????

}

glm::vec3 dbb::line::getDotFromT(float t)
{
	glm::vec3 ret;
	ret.x = p.x* t + M.x;
	ret.y = p.y*t + M.y;
	ret.z = p.z*t + M.z;
	return ret;
}
