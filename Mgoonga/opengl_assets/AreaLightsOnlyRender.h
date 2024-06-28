#pragma once

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class eAreaLightsOnlyRender
{
public:
	eAreaLightsOnlyRender(const std::string& vS, const std::string& fS);
	~eAreaLightsOnlyRender();

	void Render(const Camera& camera,
		const Light& light,
		const std::vector<shObject>& objects);

	Shader& GetShader() { return mainShader; }

protected:
	Shader mainShader;

	Texture m1, m2;

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;
	GLuint eyePositionWorldUniformLocation;
	GLuint shadowMatrixUniformLocation;
	GLuint BonesMatLocation;

	std::vector<glm::mat4> matrices;
};