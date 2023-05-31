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

	void Render(glm::vec2 _top_left, glm::vec2 _right_botom,
							glm::vec2 _tex_top_left, glm::vec2 _tex_right_botom,
							float viewport_width, float viewport_height);
	void RenderContrast(const Camera& camera, float blur_coef);
	void RenderFrame(glm::vec2 _top_left, glm::vec2 _right_botom, float viewport_width, float viewport_height);
	void RenderKernel();
	
	void SetRenderingFunction(int32_t);
	void SetTexture(Texture t)			    { screenMesh->SetTextureOne(t); }
	void SetTextureContrast(Texture t)	{ screenMesh->SetTextureTwo(t); }
	void SetTextureMask(Texture t)			{ screenMesh->SetTextureThree(t); }

	Shader& GetShader() { return screenShader; }
protected:
	Shader							          screenShader;
	std::unique_ptr<eFrameMesh>		frameMesh;
	std::unique_ptr<eScreenMesh>	screenMesh;

	GLuint							textureLoc;
	GLuint							frameLoc;
	GLuint							blendLoc;
	GLuint							kernelLoc;
  GLuint							blurCoefLoc;
}; 


