#pragma once
#include <glm\glm\glm.hpp>
#include <vector>

struct SharedData {

	std::vector<glm::vec3> vec_data{ glm::vec3() ,glm::vec3() };
	SharedData() {  }
	
};

