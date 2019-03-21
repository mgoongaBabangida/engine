#include "stdafx.h"
#include "WaveRender.h"
#include "TerrainModel.h"
#include "Structures.h"

eWaveRender::eWaveRender(std::unique_ptr<TerrainModel> model, 
						Texture* tex,
						Texture* normals, 
						Texture* Height, 
						const std::string& vS, 
						const std::string& fS)
{
	wave_shader.installShaders(vS.c_str(), fS.c_str());

	modelToWorldMatrixUniformLocation	= glGetUniformLocation(wave_shader.ID, "modelToWorldMatrix");
	fullTransformationUniformLocation	= glGetUniformLocation(wave_shader.ID, "MVP");
	modelViewMatrixLocation				= glGetUniformLocation(wave_shader.ID, "ModelViewMatrix");
	normalMatrixLocation				= glGetUniformLocation(wave_shader.ID, "NormalMatrix");
	shadowMatrixUniformLocation			= glGetUniformLocation(wave_shader.ID, "shadowMatrix");
	eyePositionWorldUniformLocation		= glGetUniformLocation(wave_shader.ID, "eyePositionWorld");
	
	glUseProgram(wave_shader.ID);
	//Light
	lightAmbientLoc		= glGetUniformLocation(wave_shader.ID, "light.ambient");
	lightDiffuseLoc		= glGetUniformLocation(wave_shader.ID, "light.diffuse");
	lightSpecularLoc	= glGetUniformLocation(wave_shader.ID, "light.specular");
	lightPosLoc			= glGetUniformLocation(wave_shader.ID, "light.position");

	//Material
	matAmbientLoc	= glGetUniformLocation(wave_shader.ID, "material.ambient");
	matDiffuseLoc	= glGetUniformLocation(wave_shader.ID, "material.texture_diffuse1");
	matSpecularLoc	= glGetUniformLocation(wave_shader.ID, "material.texture_specular1");
	matShineLoc		= glGetUniformLocation(wave_shader.ID, "material.shininess");

	glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f); //0.5f, 0.5f, 0.5f
	glUniform1f(matShineLoc, 32.0f); //32.0f

	TimeFactorLoc	= glGetUniformLocation(wave_shader.ID, "Time");
	clock.start();
	
	m_model.swap(model);
	m_model->initialize(tex, tex);
	object.reset(new eObject((IModel*)m_model.get()));
	object->getTransform()->setTranslation(glm::vec3(3.0f, 2.0f, 0.0f));
	object->getTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	object->getTransform()->setRotation(str::PI / 2, 0.0f, 0.0f);
}

void eWaveRender::Render(const glm::mat4&	ProjectionMatrix, 
						const Camera&		camera, 
						const Light&		light,
						const glm::mat4&	ShadowMatrix,
						std::vector<Flag>	flags)
{
	glUseProgram(wave_shader.ID);

	glm::mat4 worldToProjectionMatrix	= ProjectionMatrix * camera.getWorldToViewMatrix();
	glm::mat4 modelViewMatrix			= camera.getWorldToViewMatrix() * object->getTransform()->getModelMatrix();

	glUniform3f(lightAmbientLoc, light.ambient.x, light.ambient.y, light.ambient.z);  //!? diffuse
	glUniform3f(lightDiffuseLoc, light.diffuse.x, light.diffuse.y, light.diffuse.z);
	glUniform3f(lightSpecularLoc, light.specular.x, light.specular.y, light.specular.z);
	glUniform4f(lightPosLoc, light.light_vector.x, light.light_vector.y, light.light_vector.z, light.light_vector.w);

	glUniform1f(glGetUniformLocation(wave_shader.ID, "light.constant"), 1.0f); // transfer to light
	glUniform1f(glGetUniformLocation(wave_shader.ID, "light.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(wave_shader.ID, "light.quadratic"), 0.032f);
	//light end


	glm::vec3 eyePosition = camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&ShadowMatrix[0][0]);  //shadow

	glUniform1f(TimeFactorLoc, time);
	int msc = clock.newFrame();
	float dur = (float)msc / 1000.0f;
	time += dur;

	glDisable(GL_CULL_FACE);
	glUniform1i(glGetUniformLocation(wave_shader.ID, "normalMapping"), GL_FALSE);
	for (auto& flag : flags)
	{
		object->getTransform()->setTranslation(flag.position);
		object->getTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
		object->getTransform()->billboard(-camera.getDirection());
		glm::quat cur = object->getTransform()->getRotation();
		glm::quat plus = glm::toQuat(glm::rotate(mat4(),(float) str::PI / 2, glm::vec3(1, 0, 0)));
		object->getTransform()->setRotation(cur * plus);

		m_model->setDiffuse(flag.tex);
		m_model->setSpecular(flag.tex);

		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->getTransform()->getModelMatrix()[0][0]);
		glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, &modelViewMatrix[0][0]);
		/*glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE,
		&glm::mat3(glm::vec3(modelViewMatrix[0]), glm::vec3(modelViewMatrix[1]), glm::vec3(modelViewMatrix[2]))[0][0]);*/

		object->getModel()->Draw();
	}
	glUniform1i(glGetUniformLocation(wave_shader.ID, "normalMapping"), GL_TRUE);
	glEnable(GL_CULL_FACE);
}
