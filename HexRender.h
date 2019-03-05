#pragma once
#include <glm\glm.hpp>
#include <qobject.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "Shader.h"
#include <vector>

class eHexRender
{
	Shader hex_shader;
	
	GLuint hexVAO;
	GLuint hexVBO;
	GLuint MVPLoc;

	std::vector<glm::vec3> dots;
public:
	eHexRender();
	void Render(glm::mat4 mvp);
};
