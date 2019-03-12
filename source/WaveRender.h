#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"
#include "Clock.h"
#include <memory>
#include <vector>

class TerrainModel;

class eWaveRender
{
public:
	eWaveRender(std::unique_ptr<TerrainModel> model, Texture* tex, Texture* normals, Texture* Height);
	void Render(const glm::mat4&  ProjectionMatrix,
				const Camera&	  camera,
				const Light&	  light,
				const glm::mat4&  ShadowMatrix,
				std::vector<Flag> flags);

private:
	Shader							wave_shader;
	std::unique_ptr<eObject>		object;
	std::unique_ptr<TerrainModel>	m_model;
	
	Clock							clock;
	float							time		= 0.0f;
	float							Freq		= 2.5f;
	float							Velocity	= 2.5f;
	float							Amp			= 0.6f;
	//
	GLuint fullTransformationUniformLocation;  
	GLuint modelToWorldMatrixUniformLocation; 
	GLuint modelViewMatrixLocation;  
	GLuint normalMatrixLocation; 
	GLuint normalMapingLocation; 
	GLuint eyePositionWorldUniformLocation; //
	GLuint shadowMatrixUniformLocation;  //
	GLuint matAmbientLoc;
	GLuint matDiffuseLoc;
	GLuint matSpecularLoc;
	GLuint matShineLoc;
	GLuint lightAmbientLoc;
	GLuint lightDiffuseLoc;
	GLuint lightSpecularLoc;
	GLuint lightPosLoc;
	GLuint TimeFactorLoc;
};

