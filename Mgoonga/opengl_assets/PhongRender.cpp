#include "stdafx.h"
#include "PhongRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//---------------------------------------------------------------------------------
ePhongRender::ePhongRender(const std::string& vS, const std::string& fS)
: matrices(MAX_BONES)
{
	mainShader.installShaders(vS.c_str(), fS.c_str()); //main pass

	glUseProgram(mainShader.ID());

	//Material
	GLuint matAmbientLoc		= glGetUniformLocation(mainShader.ID(), "material.ambient");
	GLuint matDiffuseLoc		= glGetUniformLocation(mainShader.ID(), "material.texture_diffuse1");
	GLuint matSpecularLoc		= glGetUniformLocation(mainShader.ID(), "material.texture_specular1");
	GLuint matShineLoc			= glGetUniformLocation(mainShader.ID(), "material.shininess");

	//@todo Need to delete this default and use it correctly
	glUniform3f(matAmbientLoc, 0.5f, 0.5f, 0.5f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f); //0.5f, 0.5f, 0.5f
	glUniform1f(matShineLoc, 32.0f); //32.0f

	//Matrix
	fullTransformationUniformLocation	= glGetUniformLocation(mainShader.ID(), "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(mainShader.ID(), "modelToWorldMatrix");
	shadowMatrixUniformLocation			= glGetUniformLocation(mainShader.ID(), "shadowMatrix"); //shadow
	eyePositionWorldUniformLocation		= glGetUniformLocation(mainShader.ID(), "eyePositionWorld");
	BonesMatLocation = glGetUniformLocation(mainShader.ID(), "gBones");

	LightingIndexDirectional = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongDirectionalSpecDif");
	LightingIndexPoint = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongPointSpecDif");
	LightingIndexSpot = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongFlashSpecDif");
}

//---------------------------------------------------------------------------------
ePhongRender::~ePhongRender()
{
}

//-----------------------------------------------------------------------------------------------------
void ePhongRender::Render(const Camera&								camera,
						             const Light&									light,
						             const std::vector<shObject>&	objects)
{
	glUseProgram(mainShader.ID());

	mainShader.SetUniformData("debug_white_color", m_debug_white);
	mainShader.SetUniformData("debug_white_texcoords", m_debug_text_coords);
	mainShader.SetUniformData("gamma_correction", m_gamma_correction);
	mainShader.SetUniformData("tone_mapping", m_tone_mapping);
	mainShader.SetUniformData("hdr_exposure", m_exposure);
	mainShader.SetUniformData("ssao_threshold", m_ssao_threshold);
	mainShader.SetUniformData("ssao_strength", m_ssao_strength);

	mainShader.SetUniformData("light.ambient", light.ambient);
	mainShader.SetUniformData("light.diffuse", light.diffuse);
	mainShader.SetUniformData("light.specular", light.specular);
	mainShader.SetUniformData("light.position", light.light_position);
	mainShader.SetUniformData("light.direction", light.light_direction);

	mainShader.SetUniformData("light.constant", light.constant);
	mainShader.SetUniformData("light.linear", light.linear);
	mainShader.SetUniformData("light.quadratic", light.quadratic);
	mainShader.SetUniformData("light.cutOff", light.cutOff);
	mainShader.SetUniformData("light.outerCutOff", light.outerCutOff);

	if (light.type == eLightType::POINT)
	{
		mainShader.SetUniformData("shininess", 32.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(light.light_direction),
																							glm::vec3(0.0f, 1.0f, 0.0f));
		mainShader.SetUniformData("shadow_directional", false);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexPoint);
		shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
	}
	else if(light.type == eLightType::SPOT)
	{
		mainShader.SetUniformData("shininess", 32.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(light.light_direction),
																							glm::vec3(0.0f, 1.0f, 0.0f));
		mainShader.SetUniformData("shadow_directional", true);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexSpot);
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::DIRECTION)
	{
		mainShader.SetUniformData("shininess", 64.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position),
																							glm::vec3(0.0f, 0.0f, 0.0f), /*glm::vec3(light.light_position) + light.light_direction,*/
																							glm::vec3(0.0f, 1.0f, 0.0f));
		mainShader.SetUniformData("shadow_directional", true);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexDirectional);
    shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);  //shadow
	glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);
	mainShader.SetUniformData("far_plane", camera.getFarPlane());

	glm ::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	for (auto &object : objects)
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);

		if (object->IsTextureBlending())
			mainShader.SetUniformData("texture_blending", true);
		else
			mainShader.SetUniformData("texture_blending", false);

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

//---------------------------------------------------------------------------
void ePhongRender::SetClipPlane(float Height)
{
	glUseProgram(mainShader.ID());
	GLuint clipPlaneLoc = glGetUniformLocation(mainShader.ID(), "clip_plane");
	glUniform4f(clipPlaneLoc, 0, 1, 0, Height);
}

//---------------------------------------------------------------------------
void ePhongRender::SetShadowMatrix(glm::mat4 shadow_matrix)
{
	glUseProgram(mainShader.ID());
	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&shadow_matrix[0][0]);  //shadow
}

