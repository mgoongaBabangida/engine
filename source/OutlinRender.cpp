#include "stdafx.h"
#include "OutlinRender.h"

eOutlineRender::eOutlineRender(const std::string& vS, const std::string& fS)
{
	shader.installShaders(vS.c_str(), fS.c_str());

	fullTransformationUniformLocation	= glGetUniformLocation(shader.ID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(shader.ID, "modelToWorldMatrix");
	eyePositionWorldUniformLocation		= glGetUniformLocation(shader.ID, "eyePositionWorld");
}

void eOutlineRender::Render(const glm::mat4 & ProjectionMatrix, const Camera & camera, const Light & light, const glm::mat4 & ShadowMatrix, std::vector<shObject>& objects)
{
	glUseProgram(shader.ID);

	glm::mat4 viewToProjectionMatrix	= ProjectionMatrix;
	glm::mat4 worldToViewMatrix			= camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectionMatrix	= viewToProjectionMatrix * worldToViewMatrix;

	glm::vec3 eyePosition =camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	for (auto &object : objects)
	{
		object->getTransform()->incrementScale();
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->getTransform()->getModelMatrix()[0][0]);
		//*********************
		std::vector<glm::mat4> matrices(100);
		if (object->getRigger() != nullptr)
		{
			matrices = object->getRigger()->GetMatrices();
		}
		int loc = glGetUniformLocation(shader.ID, "gBones");
		glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);
		//*********************
		object->getModel()->Draw();
		object->getTransform()->decrementScale();
	}
}
