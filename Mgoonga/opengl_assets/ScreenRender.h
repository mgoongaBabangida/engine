#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>
#include "ScreenMesh.h"

//---------------------------------------------------------------------------
class eScreenRender
{
public:
	eScreenRender(Texture tex, const std::string& vS, const std::string& fS);

	void Render(const Camera& camera);
	void RenderContrast(const Camera& camera, float blur_coef);
	void RenderFrame(glm::vec2 _top_left, glm::vec2 _right_botom, float viewport_width, float viewport_height);
	
	void SetTexture(Texture t)			    { screenMesh->SetTextureOne(t); }
	void SetTextureContrast(Texture t)	{ screenMesh->SetTextureTwo(t); }

protected:
	Shader							          screenShader;
	std::unique_ptr<eFrameMesh>		frameMesh;
	std::unique_ptr<eScreenMesh>	screenMesh;

	GLuint							textureLoc;
	GLuint							frameLoc;
	GLuint							blendLoc;
  GLuint              blurCoefLoc;
}; 


