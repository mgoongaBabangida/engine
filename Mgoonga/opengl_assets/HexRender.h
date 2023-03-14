#pragma once
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include "Shader.h"

//----------------------------------------------------
class eHexRender
{
public:
	eHexRender(const std::string& vS, const std::string& fS, const std::string&	gS, std::vector<glm::vec3>, float);
  ~eHexRender();

	void Render(glm::mat4 mvp);

	Shader& GetShader() { return hex_shader; }
private:
	Shader hex_shader;

	GLuint hexVAO;
	GLuint hexVBO;
	GLuint MVPLoc;
	GLuint radiusLoc;
	float radius;
	std::vector<glm::vec3> dots;
};
