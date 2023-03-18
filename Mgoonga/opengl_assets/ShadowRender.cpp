#include "stdafx.h"
#include "ShadowRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//-----------------------------------------------------------------------------
eShadowRender::eShadowRender(const std::string& vS, const std::string& fS, const std::string& gSP, const std::string& fSP)
	: matrices(100)
{
	shaderDir.installShaders(vS.c_str(), fS.c_str());
	MVPUniformLocationDir				= glGetUniformLocation(shaderDir.ID(), "MVP");
	
	shaderPoint.installShaders(vS.c_str(), fSP.c_str(), gSP.c_str());
	ModelUniformLocationPoint			= glGetUniformLocation(shaderPoint.ID(), "MVP");
	ProjectionTransformsUniformLocation = glGetUniformLocation(shaderPoint.ID(), "shadowMatrices");
	FarPlaneUniformLocation = glGetUniformLocation(shaderPoint.ID(), "far_plane");
}

//-----------------------------------------------------------------------------
void eShadowRender::Render(const Camera&					camera,
													 const Light&						light,
													 std::vector<shObject>&	objects)
{
	glm::mat4 ModelToWorldMatrix = glm::translate(glm::vec3(light.light_position));
	
	if (light.type == eLightType::POINT)
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)camera.getWidth()/ camera.getHeight(), camera.getNearPlane(), camera.getFarPlane()); //should be 90
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		
		glUseProgram(shaderPoint.ID());
		glUniformMatrix4fv(ProjectionTransformsUniformLocation, 6, GL_FALSE, &shadowTransforms[0][0][0]);
		glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
		//RENDER DEPTH
		for (auto &object : objects)
		{
			glm::mat4 modelMatrix = object->GetTransform()->getModelMatrix();
			glUniformMatrix4fv(ModelUniformLocationPoint, 1, GL_FALSE, &modelMatrix[0][0]);
			object->GetModel()->Draw();
		}
	}
	else if (light.type == eLightType::DIRECTION)
	{
		glUseProgram(shaderDir.ID());
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
		//RENDER DEPTH
		for (auto &object : objects)
		{
			glm::mat4 modelToProjectionMatrix = shadowMatrix * object->GetTransform()->getModelMatrix();
			glUniformMatrix4fv(MVPUniformLocationDir, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
			object->GetModel()->Draw();
		}
	}
	else // cut off
	{
		assert(false, "");
	}
}
