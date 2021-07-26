#pragma once

#include "Shader.h"
#include <base/base.h>
#include <math/Clock.h>

#include "Texture.h"
#include <math/Camera.h>

#include <memory>
#include <vector>

class TerrainModel;

//----------------------------------------------------------
class eWaveRender
{
public:
	eWaveRender(std::unique_ptr<TerrainModel> model, 
				Texture*						tex, 
				Texture*						normals, 
				Texture*						Height,
				const std::string&				vS, 
				const std::string&				fS);

	void Render(const Camera&	  camera,
				const Light&	  light,
				std::vector<eObject*> flags);

private:
	Shader							wave_shader;
	std::unique_ptr<eObject>		object;
	std::unique_ptr<TerrainModel>	m_model;
	
	math::eClock					clock;
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


