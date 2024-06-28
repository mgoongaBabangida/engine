#include "stdafx.h"

#include "AreaLightsOnlyRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>
#include "LTC.h"

//---------------------------------------------------------------------------------
eAreaLightsOnlyRender::eAreaLightsOnlyRender(const std::string& vS, const std::string& fS)
  : matrices(MAX_BONES)
{
  mainShader.installShaders(vS.c_str(), fS.c_str()); //main pass

  glUseProgram(mainShader.ID());

	//Matrix
	fullTransformationUniformLocation = glGetUniformLocation(mainShader.ID(), "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(mainShader.ID(), "modelToWorldMatrix");
	shadowMatrixUniformLocation = glGetUniformLocation(mainShader.ID(), "shadowMatrix"); //shadow
	BonesMatLocation = glGetUniformLocation(mainShader.ID(), "gBones");

	m1.TextureFromBuffer<GLfloat>(LTC1, 64, 64, GL_RGBA, GL_NEAREST);
	m2.TextureFromBuffer<GLfloat>(LTC2, 64, 64, GL_RGBA, GL_NEAREST);
}

//---------------------------------------------------------------------------------
eAreaLightsOnlyRender::~eAreaLightsOnlyRender()
{
}

//---------------------------------------------------------------------------------
void eAreaLightsOnlyRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
	glUseProgram(mainShader.ID());

	GLuint pointsLoc = glGetUniformLocation(mainShader.ID(), "areaLight.points");
	glUniform4fv(pointsLoc, 4, &_light.points[0][0]);

	mainShader.SetUniformData("areaLight.intensity", _light.intensity.x);
	mainShader.SetUniformData("areaLight.color", glm::vec4 {1.0f, 1.0f, 0.0f, 1.0f});
	mainShader.SetUniformData("areaLight.twoSided", true);

	mainShader.SetUniformData("areaLightTranslate", _light.light_position);
	mainShader.SetUniformData("eyePositionWorld", glm::vec4(_camera.getPosition(), 1.0f));
	mainShader.SetUniformData("far_plane", _camera.getFarPlane());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m1.m_id);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m2.m_id);

	glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
	for (auto& object : _objects)
	{
		mainShader.SetUniformData("roughness", object->GetModel()->GetMaterial()->roughness);
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);

		if (object->GetRigger() != nullptr)
		{
			matrices = object->GetRigger()->GetMatrices();
		}
		else
		{
			for (auto& m : matrices)
				m = UNIT_MATRIX;
		}
		glUniformMatrix4fv(BonesMatLocation, MAX_BONES, GL_FALSE, &matrices[0][0][0]);
		object->GetModel()->Draw();
	}
}
