#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <assert.h>
#include <exception>

using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace dbb
{

	class plane
	{
	public:
		float A;
		float B;
		float C;
		float D;

		plane(float a, float b, float c, float d);
		plane(glm::vec3 dot1, glm::vec3 dot2, glm::vec3 dot3);
		plane(glm::mat3 dots);
		bool isOn(glm::vec3 dot);
		bool isSame(plane other);
	};

	class line
	{
	public:
		glm::vec3 M; // dot
		glm::vec3 p; //vector direction

		line(glm::vec3 dot, glm::vec3 direction);
		bool isOn(glm::vec3 dot);
		float findT(glm::vec3 dot);
		glm::vec3 getDotFromT(float t);
		line() :M(glm::vec3()), p(glm::vec3()) {};

	};


	glm::vec3 intersection(dbb::plane P, dbb::line L);

	bool IsInside(glm::mat3 triangle, glm::vec3 dot);

}

//http://trivial-programming.blogspot.de/2009/04/v1.html


