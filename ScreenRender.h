#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Structures.h"
#include "ScreenMesh.h"

class eScreenRender
{
public:
	eScreenRender(Texture tex, const std::string& vS, const std::string& fS);

	void Render(const glm::mat4& projectionMatrix, const Camera& camera);
	void RenderContrast(const glm::mat4& projectionMatrix, const Camera& camera);
	void RenderFrame();
	
	void SetTexture(Texture t)			{ screenMesh->SetTextureOne(t); }
	void SetTextureContrast(Texture t)	{ screenMesh->SetTextureTwo(t); }

protected:
	Shader							screenShader;
	std::unique_ptr<eFrameMesh>		frameMesh;
	std::unique_ptr<eScreenMesh>	screenMesh;

	GLuint							textureLoc;
	GLuint							frameLoc;
	GLuint							blendLoc;
}; 

