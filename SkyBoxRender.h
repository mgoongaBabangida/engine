#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Structures.h"
#include "SkyBoxMesh.h"

class eSkyBoxRender
{
public:
	eSkyBoxRender(Texture cubeTexture);
	void Render(const glm::mat4& projectionMatrix, const Camera& camera);	

protected:
	Shader							skyboxShader;
	std::unique_ptr<eSkyBoxMesh>	skyBox;
	GLuint							viewLoc;
	GLuint							projectionLoc;
	GLuint							skyboxLoc;
};

