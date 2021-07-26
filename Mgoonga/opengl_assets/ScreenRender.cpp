#include "stdafx.h"
#include "ScreenRender.h"

eScreenRender::eScreenRender(Texture tex, const std::string& vS, const std::string& fS)
{
	screenShader.installShaders(vS.c_str(), fS.c_str());
	screenMesh.reset(new eScreenMesh(tex, tex));
	frameMesh.reset(new eFrameMesh);

	textureLoc	= glGetUniformLocation(screenShader.ID, "screenTexture");
	frameLoc	= glGetUniformLocation(screenShader.ID, "frame");
	blendLoc	= glGetUniformLocation(screenShader.ID, "blend");
  blurCoefLoc = glGetUniformLocation(screenShader.ID, "blurCoef");
}

void eScreenRender::Render(const Camera& camera)
{
	glUseProgram(screenShader.ID);
	glUniform1i(frameLoc, GL_FALSE);
  glUniform1i(blendLoc, GL_FALSE);
	screenMesh->Draw();
}

void eScreenRender::RenderContrast(const Camera& camera, float blur_coef)
{
	glUseProgram(screenShader.ID);
  glUniform1f(blurCoefLoc, blur_coef);
	glUniform1i(frameLoc, GL_FALSE);
	glUniform1i(blendLoc, GL_TRUE);
	screenMesh->Draw();
}

void eScreenRender::RenderFrame(glm::vec2 _top_left, glm::vec2 _right_botom, float viewport_width, float viewport_height)
{
	glUseProgram(screenShader.ID);
	glUniform1i(frameLoc, GL_TRUE);
	frameMesh->UpdateFrame(_top_left.x, _top_left.y, _right_botom.x, _right_botom.y, viewport_width, viewport_height);
	frameMesh->Draw();
}

