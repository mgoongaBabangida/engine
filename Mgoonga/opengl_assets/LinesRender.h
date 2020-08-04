#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>

//-------------------------------------------------------------------
class eLinesRender
{
public:
	eLinesRender(const std::string& vertexShaderPath,
				 const std::string& fragmentShaderPath);
	void Render(const Camera& camera, const std::vector<glm::vec3>&, std::vector<GLuint>);

protected:
	Shader	linesShader;
	GLuint	viewLoc;
	GLuint	projectionLoc;
};