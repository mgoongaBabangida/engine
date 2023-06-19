#pragma once

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>
#include <math/Timer.h>

#include <memory>

class MyModel;
class eObject;
struct Texture;

//--------------------------------------------------------------------------
class eWaterRender 
{
public:
	eWaterRender(std::unique_ptr<MyModel> model, 
				const Texture* waves,
				const Texture* DUDV,
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath);
	~eWaterRender();

	void						Render(const Camera&	camera,
												 const Light&		light);
	
	Shader& GetShader() { return waterShader; }

	float& WaveSpeedFactor() { return wave_speed_fator; }

protected:
	Shader										waterShader;

	std::unique_ptr<eObject>	object;
	MyModel*									water_model = nullptr;

	GLuint						fullTransformationUniformLocation;
	GLuint						modelToWorldMatrixUniformLocation;
	GLuint						WaterFactorLoc;
	GLuint						CameraPosLoc;
	GLuint						lightPosLoc;
	GLuint						lightColorLoc;
	GLuint						clipPlaneLoc;
	
	math::eClock		clock;
	float						move_factor	= 0.0f;
	float						waterHeight = 2.0f;
	float						wave_speed_fator = 100'000.0f;
};
