#include "stdafx.h"
#include "MainRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//---------------------------------------------------------------------------------
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
	lightDirLoc			= glGetUniformLocation(mainShader.ID, "light.direction");
	lightTypeLoc		= glGetUniformLocation(mainShader.ID, "shadow_directional");

	//Debug
	DebugWhiteLoc = glGetUniformLocation(mainShader.ID, "debug_white_color");
	DebugTexcoordsLoc = glGetUniformLocation(mainShader.ID, "debug_white_texcoords");

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
	FarPlaneUniformLocation				= glGetUniformLocation(mainShader.ID, "far_plane");
	
	LightingIndexPoint		 = glGetSubroutineIndex(mainShader.ID, GL_FRAGMENT_SHADER, "calculatePhongPointSpecDif");
	LightingIndexDirectional = glGetSubroutineIndex(mainShader.ID, GL_FRAGMENT_SHADER, "calculatePhongDirectionalSpecDif");

	glUniform1f(glGetUniformLocation(mainShader.ID, "light.constant"), 1.0f); // transfer to light
	glUniform1f(glGetUniformLocation(mainShader.ID, "light.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(mainShader.ID, "light.quadratic"), 0.032f);
}

//-----------------------------------------------------------------------------------------------------
void eMainRender::Render(const Camera&			    camera,
						             const Light&			      light,
						             std::vector<shObject>&	objects,
                         bool                   debug_white,
                         bool                   debug_text_coords)
{
	glUseProgram(mainShader.ID);

	glUniform1i(DebugWhiteLoc, debug_white);
	glUniform1i(DebugTexcoordsLoc, debug_text_coords);

	glUniform3f(lightAmbientLoc,  light.ambient.x,		   light.ambient.y,	        light.ambient.z);
	glUniform3f(lightDiffuseLoc,  light.diffuse.x,		   light.diffuse.y,	        light.diffuse.z);
	glUniform3f(lightSpecularLoc, light.specular.x,		   light.specular.y,	    light.specular.z);
	glUniform4f(lightPosLoc,	  light.light_position.x,  light.light_position.y,  light.light_position.z,  light.light_position.w);
	glUniform4f(lightDirLoc,	  light.light_direction.x, light.light_direction.y, light.light_direction.z, light.light_direction.w);
	
	if (light.type == eLightType::POINT)
	{
    glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position + light.light_direction), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, 0);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexPoint);
		shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::DIRECTION)
	{
    glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, 1);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexDirectional);
    shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix; //$todo should be ortho projection
	}
	else // cut off
	{
		assert(false, "");
	}

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);  //shadow
	worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
	glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);

	for (auto &object : objects)
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);

		if(object->GetRigger() != nullptr)
			matrices = object->GetRigger()->GetMatrices();
		else
			for (auto& m : matrices)
				m = UNIT_MATRIX;

		int loc = glGetUniformLocation(mainShader.ID, "gBones");
		glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);

		object->GetModel()->Draw();
	}
}

//---------------------------------------------------------------------------
void eMainRender::SetClipPlane(float Height)
{
	glUseProgram(mainShader.ID);
	GLuint clipPlaneLoc = glGetUniformLocation(mainShader.ID, "clip_plane");
	glUniform4f(clipPlaneLoc, 0, 1, 0, Height);
}

//---------------------------------------------------------------------------
void eMainRender::SetShadowMatrix(glm::mat4 shadow_matrix)
{
	glUseProgram(mainShader.ID);
	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&shadow_matrix[0][0]);  //shadow
}

