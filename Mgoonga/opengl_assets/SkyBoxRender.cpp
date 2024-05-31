#include "stdafx.h"
#include "SkyBoxRender.h"
#include <math/Transform.h>

eSkyBoxRender::eSkyBoxRender(const Texture*			_texture, 
							const std::string&  vertexShaderPath, 
							const std::string&  fragmentShaderPath)
{
	skyboxShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	skyBox.reset(new eSkyBoxMesh(_texture));

	viewLoc			= glGetUniformLocation(skyboxShader.ID(), "view");
	projectionLoc	= glGetUniformLocation(skyboxShader.ID(), "projection");
	skyboxLoc		= glGetUniformLocation(skyboxShader.ID(), "skybox");
}

void eSkyBoxRender::Render(const Camera& _camera)
{
	glUseProgram(skyboxShader.ID());
	if(rotate_skybox)
		moveFactor += 0.001f;

	glm::mat4 trans = glm::toMat4(glm::quat(glm::vec3(0, moveFactor, 0.0f)));
	glm::mat4 view	= glm::mat4(glm::mat3(_camera.getWorldToViewMatrix())) * trans;	// Remove any translation component of the view matrix

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &_camera.getProjectionMatrix()[0][0]);
	glUniform1i(skyboxLoc, 5);

	skyBox->Draw();
}

void eSkyBoxRender::SetSkyBoxTexture(const Texture* _t)
{
	skyBox->SetTexture(_t);
}

const Texture* eSkyBoxRender::GetSkyBoxTexture() const
{
	return skyBox->GetTexture();
}
