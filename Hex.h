#pragma once
#include "PlaneLine.h"
#include <vector>

class Hex
{
	float radius = 0.0f;
	glm::vec2 center ;
	float Height = 0.0f;
	float common_height = 0.0f;
public:
	Hex(glm::vec2 c, float h, float r = 0.0f) :radius(r), center(c), Height(h) {}
	bool IsOn(float x, float z)
	{
		glm::vec3 center = glm::vec3(center.x,common_height,center.y);
		
		std::vector<glm::vec3> dots{
		glm::vec3(glm::sin(glm::radians(30.0f)) * radius, common_height, glm::cos(glm::radians(30.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(90.0f)) * radius, common_height, glm::cos(glm::radians(90.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(150.0f)) * radius, common_height, glm::cos(glm::radians(150.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(210.0f)) * radius, common_height, glm::cos(glm::radians(210.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(270.0f)) * radius, common_height, glm::cos(glm::radians(270.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(330.0f)) * radius, common_height, glm::cos(glm::radians(330.0f)) * radius),
			glm::vec3(glm::sin(glm::radians(30.0f)) * radius, common_height, glm::cos(glm::radians(30.0f)) * radius)
			 };

		std::vector<glm::mat3> triangles;
		for (int i = 0; i < 6; ++i)
		{
			glm::mat3  mat(center, center + dots[i], center + dots[i+1]) ;
			if (dbb::IsInside(mat, glm::vec3(x, common_height, z)))
				return true;
		}
		return false;
	}
	
};