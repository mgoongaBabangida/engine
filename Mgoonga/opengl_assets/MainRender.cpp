#include "stdafx.h"
#include "MainRender.h"
#include <math/Transform.h>
#include <math/Rigger.h>

//---------------------------------------------------------------------------------
eMainRender::eMainRender(const std::string& vS, const std::string& fS)
: matrices(MAX_BONES)
{
	mainShader.installShaders(vS.c_str(), fS.c_str()); //main pass

	glUseProgram(mainShader.ID());
	
	//Light
	lightAmbientLoc		= glGetUniformLocation(mainShader.ID(), "light.ambient");
	lightDiffuseLoc		= glGetUniformLocation(mainShader.ID(), "light.diffuse");
	lightSpecularLoc	= glGetUniformLocation(mainShader.ID(), "light.specular");
	lightPosLoc			= glGetUniformLocation(mainShader.ID(), "light.position");
	lightDirLoc			= glGetUniformLocation(mainShader.ID(), "light.direction");
	lightTypeLoc		= glGetUniformLocation(mainShader.ID(), "shadow_directional");

	//Debug
	DebugWhiteLoc = glGetUniformLocation(mainShader.ID(), "debug_white_color");
	DebugTexcoordsLoc = glGetUniformLocation(mainShader.ID(), "debug_white_texcoords");
	GammaCorrectionLoc = glGetUniformLocation(mainShader.ID(), "gamma_correction");
	ToneMappingLoc = glGetUniformLocation(mainShader.ID(), "tone_mapping");
	HdrExposureLoc = glGetUniformLocation(mainShader.ID(), "hdr_exposure");

	//Material
	matAmbientLoc		= glGetUniformLocation(mainShader.ID(), "material.ambient");
	matDiffuseLoc		= glGetUniformLocation(mainShader.ID(), "material.texture_diffuse1");
	matSpecularLoc		= glGetUniformLocation(mainShader.ID(), "material.texture_specular1");
	matShineLoc			= glGetUniformLocation(mainShader.ID(), "material.shininess");

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
	FarPlaneUniformLocation				= glGetUniformLocation(mainShader.ID(), "far_plane");
	BonesMatLocation = glGetUniformLocation(mainShader.ID(), "gBones");

	LightingIndexDirectional = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongDirectionalSpecDif");
	LightingIndexPoint = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongPointSpecDif");
	LightingIndexSpot = glGetSubroutineIndex(mainShader.ID(), GL_FRAGMENT_SHADER, "calculateBlinnPhongFlashSpecDif");
}

//---------------------------------------------------------------------------------
eMainRender::~eMainRender()
{
}

//-----------------------------------------------------------------------------------------------------
void eMainRender::Render(const Camera&								camera,
						             const Light&									light,
						             const std::vector<shObject>&	objects,
                         bool													debug_white,
                         bool													debug_text_coords,
												 bool													gamma_correction,
												 bool													tone_mapping,
												 float												exposure)
{
	glUseProgram(mainShader.ID());

	glUniform1i(DebugWhiteLoc, debug_white);
	glUniform1i(DebugTexcoordsLoc, debug_text_coords);
	glUniform1i(GammaCorrectionLoc, gamma_correction);
	glUniform1i(ToneMappingLoc, tone_mapping);
	glUniform1f(HdrExposureLoc, exposure);

	glUniform3f(lightAmbientLoc,  light.ambient.x,		   light.ambient.y,	        light.ambient.z);
	glUniform3f(lightDiffuseLoc,  light.diffuse.x,		   light.diffuse.y,	        light.diffuse.z);
	glUniform3f(lightSpecularLoc, light.specular.x,		   light.specular.y,	    light.specular.z);
	glUniform4f(lightPosLoc,			light.light_position.x,  light.light_position.y,  light.light_position.z,  light.light_position.w);
	glUniform3f(lightDirLoc,			light.light_direction.x, light.light_direction.y, light.light_direction.z);
	
	glUniform1f(glGetUniformLocation(mainShader.ID(), "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(mainShader.ID(), "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(mainShader.ID(), "light.quadratic"), light.quadratic);
	glUniform1f(glGetUniformLocation(mainShader.ID(), "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(mainShader.ID(), "light.outerCutOff"), light.outerCutOff);

	if (light.type == eLightType::POINT)
	{

		glUniform1f(glGetUniformLocation(mainShader.ID(), "shininess"), 16.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(light.light_position) + light.light_direction,
																							glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, false);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexPoint);
		shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
	}
	else if(light.type == eLightType::SPOT)
	{
		glUniform1f(glGetUniformLocation(mainShader.ID(), "shininess"), 16.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position), glm::vec3(0.0f, -1.0f, 0.0f),
																							glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, true);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexSpot);
		shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}
	else if (light.type == eLightType::DIRECTION)
	{
		glUniform1f(glGetUniformLocation(mainShader.ID(), "shininess"), 64.0f);
		glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_position),
																							glm::vec3(0.0f, 0.0f, 0.0f), /*glm::vec3(light.light_position) + light.light_direction,*/
																							glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform1i(lightTypeLoc, true);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &LightingIndexDirectional);
    shadowMatrix = camera.getProjectionOrthoMatrix() * worldToViewMatrix;
	}

	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);  //shadow
	glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
	glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);

	glm ::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	for (auto &object : objects)
	{
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
		glUniformMatrix4fv(BonesMatLocation, 300, GL_FALSE, &matrices[0][0][0]);
		object->GetModel()->Draw();
	}
}

//---------------------------------------------------------------------------
void eMainRender::SetClipPlane(float Height)
{
	glUseProgram(mainShader.ID());
	GLuint clipPlaneLoc = glGetUniformLocation(mainShader.ID(), "clip_plane");
	glUniform4f(clipPlaneLoc, 0, 1, 0, Height);
}

//---------------------------------------------------------------------------
void eMainRender::SetShadowMatrix(glm::mat4 shadow_matrix)
{
	glUseProgram(mainShader.ID());
	glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE,
		&shadow_matrix[0][0]);  //shadow
}

