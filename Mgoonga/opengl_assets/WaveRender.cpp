#include "stdafx.h"
#include "WaveRender.h"

#include <base/base.h>
#include <base/Object.h>

#include "TerrainModel.h"
#include <math/Transform.h>

//------------------------------------------------------------------------------------------
eWaveRender::eWaveRender(std::unique_ptr<TerrainModel> model,
						const Texture* tex,
						const Texture* normals,
						const Texture* Height,
						const std::string& vS,
						const std::string& fS)
{
	wave_shader.installShaders(vS.c_str(), fS.c_str());

	modelToWorldMatrixUniformLocation	= glGetUniformLocation(wave_shader.ID(), "modelToWorldMatrix");
	fullTransformationUniformLocation	= glGetUniformLocation(wave_shader.ID(), "MVP");
	modelViewMatrixLocation						= glGetUniformLocation(wave_shader.ID(), "ModelViewMatrix");
	normalMatrixLocation							= glGetUniformLocation(wave_shader.ID(), "NormalMatrix");
	shadowMatrixUniformLocation				= glGetUniformLocation(wave_shader.ID(), "shadowMatrix");
	eyePositionWorldUniformLocation		= glGetUniformLocation(wave_shader.ID(), "eyePositionWorld");
	FarPlaneUniformLocation						= glGetUniformLocation(wave_shader.ID(), "far_plane");

	glUseProgram(wave_shader.ID());
	//Light
	lightAmbientLoc = glGetUniformLocation(wave_shader.ID(), "light.ambient");
	lightDiffuseLoc = glGetUniformLocation(wave_shader.ID(), "light.diffuse");
	lightSpecularLoc = glGetUniformLocation(wave_shader.ID(), "light.specular");
	lightPosLoc = glGetUniformLocation(wave_shader.ID(), "light.position");
	lightDirLoc = glGetUniformLocation(wave_shader.ID(), "light.direction");
	lightTypeLoc = glGetUniformLocation(wave_shader.ID(), "shadow_directional");

	//Material
	matAmbientLoc	= glGetUniformLocation(wave_shader.ID(), "material.ambient");
	matDiffuseLoc	= glGetUniformLocation(wave_shader.ID(), "material.texture_diffuse1");
	matSpecularLoc	= glGetUniformLocation(wave_shader.ID(), "material.texture_specular1");
	matShineLoc		= glGetUniformLocation(wave_shader.ID(), "material.shininess");

	glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f); //0.5f, 0.5f, 0.5f
	glUniform1f(matShineLoc, 32.0f); //32.0f

	LightingIndexDirectional = glGetSubroutineIndex(wave_shader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongDirectionalSpecDif");
	LightingIndexPoint = glGetSubroutineIndex(wave_shader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongPointSpecDif");
	LightingIndexSpot = glGetSubroutineIndex(wave_shader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongFlashSpecDif");

	TimeFactorLoc	= glGetUniformLocation(wave_shader.ID(), "Time");
	clock.start();
	
	model->initialize(tex, tex);
	m_object.reset(new eObject);
	m_object->SetModel(model.release());
	m_object->SetTransform(new Transform);
	//move this outside
	m_object->GetTransform()->setTranslation(glm::vec3(3.0f, 2.0f, 0.0f));
	m_object->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
	m_object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);
}

//-------------------------------------------------------------------------------------------------
void eWaveRender::Render(const Camera&					camera,
												 const Light&						light,
												 std::vector<shObject>	flags)
{
	glUseProgram(wave_shader.ID());

	glm::mat4 worldToProjectionMatrix	= camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	glm::mat4 modelViewMatrix			= camera.getWorldToViewMatrix() * m_object->GetTransform()->getModelMatrix();

	glUniform3f(lightAmbientLoc, light.ambient.x, light.ambient.y, light.ambient.z);  //!? diffuse
	glUniform3f(lightDiffuseLoc, light.diffuse.x, light.diffuse.y, light.diffuse.z);
	glUniform3f(lightSpecularLoc, light.specular.x, light.specular.y, light.specular.z);
	glUniform4f(lightPosLoc, light.light_position.x, light.light_position.y, light.light_position.z, light.light_position.w);
	glUniform3f(lightDirLoc, light.light_direction.x, light.light_direction.y, light.light_direction.z);

	glUniform1f(glGetUniformLocation(wave_shader.ID(), "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(wave_shader.ID(), "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(wave_shader.ID(), "light.quadratic"), light.quadratic);
	glUniform1f(glGetUniformLocation(wave_shader.ID(), "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(wave_shader.ID(), "light.outerCutOff"), light.outerCutOff);
	//light end

	glm::mat4 shadowMatrix;
	if (light.type == eLightType::POINT || light.type == eLightType::SPOT)
	{
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + glm::vec3(light.light_direction),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1f(glGetUniformLocation(wave_shader.ID(), "shininess"), 32.0f);
		if (light.type == eLightType::POINT)
		{
			glUniform1i(lightTypeLoc, false);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexPoint);
		}
		else //spot
		{
			glUniform1i(lightTypeLoc, true);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexSpot);
		}
		shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::DIRECTION)
	{
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(0.0f, 0.0f, 0.0f), /*glm::vec3(light.light_position) + light.light_direction,*/
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, true);
		glUniform1f(glGetUniformLocation(wave_shader.ID(), "shininess"), 8.0f);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexDirectional);
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix; //$todo should be ortho projection or not?
	}

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&shadowMatrix[0][0]);  //shadow
	glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
	glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);

	glUniform1f(TimeFactorLoc, time);
	int32_t msc = (int32_t)clock.newFrame();
	float dur = (float)msc / 1000.0f;
	time += dur;

	glDisable(GL_CULL_FACE); //@todo transfer
	glUniform1i(glGetUniformLocation(wave_shader.ID(), "normalMapping"), GL_FALSE);

	for(auto& flag : flags)
	{
		//move this outside
		m_object->GetTransform()->setTranslation(flag->GetTransform()->getTranslation());
		m_object->GetTransform()->setScale(flag->GetTransform()->getScaleAsVector());
		m_object->GetTransform()->billboard(-camera.getDirection());
		
		glm::quat cur = m_object->GetTransform()->getRotation();
		glm::quat plus = glm::toQuat(glm::rotate(UNIT_MATRIX, (float) PI / 2, XAXIS));
		m_object->GetTransform()->setRotation(cur * plus);

		// set with texture id
		m_object->GetModel()->SetMaterial(flag->GetModel()->GetMaterial().value());

		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * m_object->GetTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &m_object->GetTransform()->getModelMatrix()[0][0]);
		glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, &modelViewMatrix[0][0]);
		/*glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE,
		&glm::mat3(glm::vec3(modelViewMatrix[0]), glm::vec3(modelViewMatrix[1]), glm::vec3(modelViewMatrix[2]))[0][0]);*/

		m_object->GetModel()->Draw();
	}

	glUniform1i(glGetUniformLocation(wave_shader.ID(), "normalMapping"), GL_TRUE);
	glEnable(GL_CULL_FACE); //todo transfer
}

