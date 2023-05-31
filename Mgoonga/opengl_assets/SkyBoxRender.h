#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>
#include "SkyBoxMesh.h"

//-------------------------------------------------------------------------
class eSkyBoxRender
{
public:
	eSkyBoxRender(const Texture*			 cubeTexture,
				  const std::string& vertexShaderPath,
				  const std::string& fragmentShaderPath);

	void Render(const Camera& camera);	
	void SetSkyBoxTexture(const Texture*);

	Shader& GetShader() { return skyboxShader; }

protected:
	Shader							          skyboxShader;
	std::unique_ptr<eSkyBoxMesh>	skyBox;
  GLuint							          viewLoc;
  GLuint							          projectionLoc;
  GLuint							          skyboxLoc;
	float							            moveFactor = 0.0f;
};

