#include "stdafx.h"
#include "ScreenRender.h"

//--------------------------------------------------------------------------
eScreenRender::eScreenRender(Texture tex, const std::string& vS, const std::string& fS)
{
	screenShader.installShaders(vS.c_str(), fS.c_str());
	screenMesh.reset(new eScreenMesh(tex, tex));
	frameMesh.reset(new eFrameMesh);

	//@todo change for uniform function pointer
	textureLoc	= glGetUniformLocation(screenShader.ID(), "screenTexture");
	frameLoc	= glGetUniformLocation(screenShader.ID(), "frame");
	blendLoc	= glGetUniformLocation(screenShader.ID(), "blend");
	kernelLoc = glGetUniformLocation(screenShader.ID(), "kernel");
  blurCoefLoc = glGetUniformLocation(screenShader.ID(), "blurCoef");
}

//--------------------------------------------------------------------------
void eScreenRender::Render(glm::vec2 _top_left, glm::vec2 _right_botom,
													 glm::vec2 _tex_top_left, glm::vec2 _tex_right_botom,
													 float viewport_width, float viewport_height)
{
	glUseProgram(screenShader.ID());
	glUniform1i(frameLoc, GL_FALSE);
  glUniform1i(blendLoc, GL_FALSE);
	glUniform1i(kernelLoc, GL_FALSE);
	screenMesh->UpdateFrame(_top_left.x,			_top_left.y,				_right_botom.x,			_right_botom.y,
													_tex_top_left.x, _tex_right_botom.y,	_tex_right_botom.x,	_tex_top_left.y,
													viewport_width, viewport_height);
	screenMesh->Draw();
	screenMesh->SetViewPortToDefault();
}

//--------------------------------------------------------------------------
void eScreenRender::RenderContrast(const Camera& camera, float blur_coef)
{
	glUseProgram(screenShader.ID());
  glUniform1f(blurCoefLoc, blur_coef);
	glUniform1i(frameLoc, GL_FALSE);
	glUniform1i(blendLoc, GL_TRUE);

	screenShader.SetUniformData("hdr_exposure", m_exposure);
	screenShader.SetUniformData("tone_mapping", m_tone_mapping);
	screenShader.SetUniformData("gamma_correction", m_gamma_correction);

	screenMesh->SetViewPortToDefault();
	screenMesh->Draw();
	glUniform1i(blendLoc, GL_FALSE);
}

//--------------------------------------------------------------------------
void eScreenRender::RenderFrame(glm::vec2 _top_left, glm::vec2 _right_botom, float viewport_width, float viewport_height)
{
	glUseProgram(screenShader.ID());
	glUniform1i(frameLoc, GL_TRUE);
	frameMesh->UpdateFrame(_top_left.x, _top_left.y, _right_botom.x, _right_botom.y, viewport_width, viewport_height);
	frameMesh->Draw();
	glUniform1i(frameLoc, GL_FALSE);
}

//--------------------------------------------------------------------------
void eScreenRender::RenderKernel()
{
	glUseProgram(screenShader.ID());
	glUniform1i(frameLoc, GL_FALSE);
	glUniform1i(blendLoc, GL_FALSE);
	glUniform1i(kernelLoc, GL_TRUE);
	screenMesh->Draw();
	glUniform1i(kernelLoc, GL_FALSE);
}

//---------------------------------------------------------------
void eScreenRender::SetRenderingFunction(int32_t _function)
{
	glUseProgram(screenShader.ID());
	if(_function == 0)
	{
		GLuint DefaultRendering = glGetSubroutineIndex(screenShader.ID(), GL_FRAGMENT_SHADER, "DefaultColor");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &DefaultRendering);
	}
	else if(_function == 1)
	{
		GLuint CursorFollowRendering = glGetSubroutineIndex(screenShader.ID(), GL_FRAGMENT_SHADER, "TestColor");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &CursorFollowRendering);
	}
	else if (_function == 2)
	{
		GLuint GreyKernelRendering = glGetSubroutineIndex(screenShader.ID(), GL_FRAGMENT_SHADER, "GreyKernelColor");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &GreyKernelRendering);
	}
	else if (_function == 3)
	{
		GLuint BlendOnMaskRendering = glGetSubroutineIndex(screenShader.ID(), GL_FRAGMENT_SHADER, "MaskBlendColor");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &BlendOnMaskRendering);
	}
}
