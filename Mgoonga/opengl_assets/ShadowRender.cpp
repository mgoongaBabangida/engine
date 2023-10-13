#include "stdafx.h"
#include "ShadowRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//-----------------------------------------------------------------------------
eShadowRender::eShadowRender(const std::string& vS, const std::string& fS, const std::string& gSP, const std::string& fSP)
	: matrices(MAX_BONES)
{
	shaderDir.installShaders(vS.c_str(), fS.c_str());
	MVPUniformLocationDir = glGetUniformLocation(shaderDir.ID(), "MVP");
	BonesMatLocationDir = glGetUniformLocation(shaderDir.ID(), "gBones");

	shaderPoint.installShaders(vS.c_str(), fSP.c_str(), gSP.c_str());
	ModelUniformLocationPoint = glGetUniformLocation(shaderPoint.ID(), "MVP");
	ProjectionTransformsUniformLocation = glGetUniformLocation(shaderPoint.ID(), "shadowMatrices");
	FarPlaneUniformLocation = glGetUniformLocation(shaderPoint.ID(), "far_plane");
	LightPosUniformLocation = glGetUniformLocation(shaderPoint.ID(), "lightPosition");
	BonesMatLocationPoint = glGetUniformLocation(shaderPoint.ID(), "gBones");
}

//-----------------------------------------------------------------------------
void eShadowRender::Render(const Camera&					camera,
													 const Light&						light,
													 std::vector<shObject>&	objects)
{
	if (light.type == eLightType::POINT)
	{
		float aspect = 1.0f; // (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT (depth buffer or viewport)
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, camera.getNearPlane(), camera.getFarPlane()); //should be 90
		glm::vec3 lightPos = light.light_position;
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * 
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glUseProgram(shaderPoint.ID());
		glUniformMatrix4fv(ProjectionTransformsUniformLocation, 6, GL_FALSE, &shadowTransforms[0][0][0]);
		glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
		glUniform3fv(LightPosUniformLocation, 1, &light.light_position[0]);

		//RENDER DEPTH
		for (auto &object : objects)
		{
			glm::mat4 modelMatrix = object->GetTransform()->getModelMatrix();
			glUniformMatrix4fv(ModelUniformLocationPoint, 1, GL_FALSE, &modelMatrix[0][0]);

			if (object->GetRigger() != nullptr)
			{
				matrices = object->GetRigger()->GetMatrices();
			}
			else
			{
				for (auto& m : matrices)
					m = UNIT_MATRIX;
			}
			glUniformMatrix4fv(BonesMatLocationPoint, MAX_BONES, GL_FALSE, &matrices[0][0][0]);

			object->GetModel()->Draw();
		}
	}
	else if (light.type == eLightType::DIRECTION || light.type == eLightType::SPOT)
	{
		glUseProgram(shaderDir.ID());
		glm::mat4 worldToViewMatrix;
		if (light.type == eLightType::DIRECTION)
		{
			worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(0.0f, 0.0f, 0.0f), /*glm::vec3(light.light_position) + light.light_direction,*/
				glm::vec3(0.0f, 1.0f, 0.0f));
			shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
		}
		else
		{
			worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + light.light_direction,
				glm::vec3(0.0f, 1.0f, 0.0f));
			shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
		}
		//RENDER DEPTH
		for (auto &object : objects)
		{
			glm::mat4 modelToProjectionMatrix = shadowMatrix * object->GetTransform()->getModelMatrix();
			glUniformMatrix4fv(MVPUniformLocationDir, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);

			if (object->GetRigger() != nullptr)
			{
				matrices = object->GetRigger()->GetMatrices();
			}
			else
			{
				for (auto& m : matrices)
					m = UNIT_MATRIX;
			}
			glUniformMatrix4fv(BonesMatLocationDir, MAX_BONES, GL_FALSE, &matrices[0][0][0]);

			object->GetModel()->Draw();
		}
	}
}
