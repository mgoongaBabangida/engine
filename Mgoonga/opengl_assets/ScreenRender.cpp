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
}

void eScreenRender::Render(const Camera& camera)
{
	glUseProgram(screenShader.ID);
	glUniform1i(frameLoc, GL_FALSE);
	screenMesh->Draw();
}

void eScreenRender::RenderContrast(const Camera& camera) // not used!
{
	glUseProgram(screenShader.ID);
	glUniform1i(frameLoc, GL_FALSE);
	glUniform1i(blendLoc, GL_TRUE);
	screenMesh->Draw();
}

void eScreenRender::RenderFrame()
{
	glUseProgram(screenShader.ID);
	glUniform1i(frameLoc, GL_TRUE);
	frameMesh->Draw();
}
