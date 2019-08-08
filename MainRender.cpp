#include "stdafx.h"
#include "MainRender.h"

eMainRender::eMainRender(const std::string& vS, const std::string& fS)
: matrices(100)
{
	mainShader.installShaders(vS.c_str(), fS.c_str()); //main pass

	glUseProgram(mainShader.ID);
	//Light
	lightAmbientLoc		= glGetUniformLocation(mainShader.ID, "light.ambient");
	lightDiffuseLoc		= glGetUniformLocation(mainShader.ID, "light.diffuse");
	lightSpecularLoc	= glGetUniformLocation(mainShader.ID, "light.specular");
	lightPosLoc			= glGetUniformLocation(mainShader.ID, "light.position");
	
	//Material
	matAmbientLoc		= glGetUniformLocation(mainShader.ID, "material.ambient");
	matDiffuseLoc		= glGetUniformLocation(mainShader.ID, "material.texture_diffuse1");
	matSpecularLoc		= glGetUniformLocation(mainShader.ID, "material.texture_specular1");
	matShineLoc			= glGetUniformLocation(mainShader.ID, "material.shininess");

	glUniform3f(matAmbientLoc, 0.5f, 0.5f, 0.5f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f); // 1.0f, 0.5f, 0.31f
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f); //0.5f, 0.5f, 0.5f
	glUniform1f(matShineLoc, 32.0f); //32.0f

	fullTransformationUniformLocation	= glGetUniformLocation(mainShader.ID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(mainShader.ID, "modelToWorldMatrix");
	shadowMatrixUniformLocation			= glGetUniformLocation(mainShader.ID, "shadowMatrix"); //shadow
	eyePositionWorldUniformLocation		= glGetUniformLocation(mainShader.ID, "eyePositionWorld");

	GLuint LightingIndex = glGetSubroutineIndex(mainShader.ID, GL_FRAGMENT_SHADER, "calculateBlindPhongPointSpecDif");
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndex);
}

void eMainRender::Render(const glm::mat4&		projectionMatrix, 
						const Camera&			camera, 
						const Light&			light, 
						const glm::mat4&		shadowMatrix, 
						std::vector<shObject>&	objects)
{
	glUseProgram(mainShader.ID);

	glUniform3f(lightAmbientLoc,  light.ambient.x,		light.ambient.y,	  light.ambient.z);
	glUniform3f(lightDiffuseLoc,  light.diffuse.x,		light.diffuse.y,	  light.diffuse.z);
	glUniform3f(lightSpecularLoc, light.specular.x,		light.specular.y,	  light.specular.z);
	glUniform4f(lightPosLoc,	  light.light_position.x, light.light_position.y, light.light_position.z, light.light_position.w);

	glUniform1f(glGetUniformLocation(mainShader.ID, "light.constant"),  1.0f); // transfer to light
	glUniform1f(glGetUniformLocation(mainShader.ID, "light.linear"),	0.09f);
	glUniform1f(glGetUniformLocation(mainShader.ID, "light.quadratic"), 0.032f);
	//light end

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);  //shadow
	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectionMatrix = projectionMatrix * worldToViewMatrix;

	glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);

	for (auto &object : objects) 
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->getTransform()->getModelMatrix()[0][0]);
		//*********************
		for(auto& m : matrices)
		{
			m = UNIT_MATRIX;
		}
		if(object->getRigger() != nullptr)
		{
			matrices = object->getRigger()->GetMatrices();
		}
		int loc = glGetUniformLocation(mainShader.ID, "gBones");
		glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);
		//*********************
		object->getModel()->Draw();
	}

	//debug light
	/*glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * lightObject->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &lightObject->getModelMatrix()[0][0]);
	lightObject->Draw(main_shader);*/
}

void eMainRender::SetClipPlane(float Height)
{
	glUseProgram(mainShader.ID);
	GLuint clipPlaneLoc = glGetUniformLocation(mainShader.ID, "clip_plane");
	glUniform4f(clipPlaneLoc, 0, 1, 0, Height);
}

void eMainRender::SetShadowMatrix(glm::mat4 shadow_matrix)
{
	glUseProgram(mainShader.ID);
	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&shadow_matrix[0][0]);  //shadow
}
