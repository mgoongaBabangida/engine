#include "stdafx.h"
#include "ScreenRender.h"

eScreenRender::eScreenRender(Texture tex)
{
	screenShader.installShaders("PostProcessingVertexShader.glsl", "PostProcessingFragmentShader.glsl"); 
	screenMesh.reset(new eScreenMesh(tex, tex));
	frameMesh.reset(new eFrameMesh);

	textureLoc	= glGetUniformLocation(screenShader.ID, "screenTexture");
	frameLoc	= glGetUniformLocation(screenShader.ID, "frame");
	blendLoc	= glGetUniformLocation(screenShader.ID, "blend");
}

void eScreenRender::Render(const glm::mat4 & ProjectionMatrix, const Camera & camera)
{
	glUseProgram(screenShader.ID);

	glUniform1i(frameLoc, GL_FALSE);

	screenMesh->Draw();

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, m_texture.id);	// Use the color attachment texture as the texture of the quad plane //reflactionFBO->GetTexture().id
	//
	//glBindVertexArray(quadVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
}

void eScreenRender::RenderContrast(const glm::mat4 & projectionMatrix, const Camera & camera) // not used!
{
	glUseProgram(screenShader.ID);

	glUniform1i(frameLoc, GL_FALSE);
	glUniform1i(blendLoc, GL_TRUE);

	screenMesh->Draw();

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture.id);	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_texContrast.id);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/
}

void eScreenRender::RenderFrame()
{
	glUseProgram(screenShader.ID);

	glUniform1i(frameLoc, GL_TRUE);

	frameMesh->Draw();

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture.id);*/

	/*glBindVertexArray(quadVAO_fr);
	glDrawArrays(GL_LINE_STRIP, 0, 5);
	glBindVertexArray(0);*/
}
