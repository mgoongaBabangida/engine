#include "SkyBoxRender.h"

eSkyBoxRender::eSkyBoxRender(Texture			_texture, 
							const std::string& vertexShaderPath, 
							const std::string& fragmentShaderPath)
{
	skyboxShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	skyBox.reset(new eSkyBoxMesh(_texture));

	viewLoc			= glGetUniformLocation(skyboxShader.ID, "view");
	projectionLoc	= glGetUniformLocation(skyboxShader.ID, "projection");
	skyboxLoc		= glGetUniformLocation(skyboxShader.ID, "skybox");
}

void eSkyBoxRender::Render(const glm::mat4& _projectionMatrix, const Camera& _camera)
{
	glUseProgram(skyboxShader.ID);

	glm::mat4 view	= glm::mat4(glm::mat3(_camera.getWorldToViewMatrix()));	// Remove any translation component of the view matrix

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &_projectionMatrix[0][0]);
	glUniform1i(skyboxLoc, 5);

	skyBox->Draw();
}
