#pragma once
#include "Shader.h"

class eHexRender
{
	Shader hex_shader;
	
	GLuint hexVAO;
	GLuint hexVBO;
	GLuint MVPLoc;

	std::vector<glm::vec3> dots;
public:
	eHexRender(const std::string& vS, const std::string& fS, const std::string&	gS);
	void Render(glm::mat4 mvp);
};
