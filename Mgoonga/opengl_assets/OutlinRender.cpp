#include "stdafx.h"
#include "OutlinRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//--------------------------------------------------------------------------------------------
eOutlineRender::eOutlineRender(const std::string& vS, const std::string& fS)
	: matrices(300)
{
	shader.installShaders(vS.c_str(), fS.c_str());

	fullTransformationUniformLocation	= glGetUniformLocation(shader.ID(), "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(shader.ID(), "modelToWorldMatrix");
	eyePositionWorldUniformLocation		= glGetUniformLocation(shader.ID(), "eyePositionWorld");
}

void eOutlineRender::Render(const Camera & camera, const Light & light, const std::vector<shObject>& objects)
{
	glUseProgram(shader.ID());

	glm::mat4 worldToProjectionMatrix	= camera.getProjectionMatrix() * camera.getWorldToViewMatrix();

	glm::vec3 eyePosition =camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	for (auto &object : objects)
	{
		object->GetTransform()->incrementScale();
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);
		//*********************
		if (object->GetRigger() != nullptr)
		{
			matrices = object->GetRigger()->GetMatrices();
		}
		else
		{
			for (auto& m : matrices)
			{
				m = UNIT_MATRIX;
			}
		}
		int loc = glGetUniformLocation(shader.ID(), "gBones");
		glUniformMatrix4fv(loc, 300, GL_FALSE, &matrices[0][0][0]);
		//*********************
		object->GetModel()->Draw();
		object->GetTransform()->decrementScale();
	}
}
