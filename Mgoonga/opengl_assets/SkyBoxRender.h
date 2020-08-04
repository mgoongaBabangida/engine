#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>
#include "SkyBoxMesh.h"

//-------------------------------------------------------------------------
class eSkyBoxRender
{
public:
	eSkyBoxRender(Texture*			 cubeTexture,
				  const std::string& vertexShaderPath,
				  const std::string& fragmentShaderPath);
	void Render(const Camera& camera);	
	void SetSkyBoxTexture(Texture*);

protected:
	Shader							skyboxShader;
	std::unique_ptr<eSkyBoxMesh>	skyBox;
	GLuint							viewLoc;
	GLuint							projectionLoc;
	GLuint							skyboxLoc;
	float							moveFactor = 0.0f;
};

