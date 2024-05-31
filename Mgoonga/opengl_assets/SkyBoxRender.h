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
	const Texture* GetSkyBoxTexture() const;

	Shader& GetShader() { return skyboxShader; }

	bool& GetRotateSkyBoxRef() { return rotate_skybox; }
protected:
	Shader							          skyboxShader;
	std::unique_ptr<eSkyBoxMesh>	skyBox;
  GLuint							          viewLoc;
  GLuint							          projectionLoc;
  GLuint							          skyboxLoc;
	float							            moveFactor = 0.0f;
	bool													rotate_skybox = true;
};

