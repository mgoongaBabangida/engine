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
				Texture* waves, 
				Texture* DUDV,
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath);
	~eWaterRender();

	void						Render(const Camera&	camera, 
									   const Light&		light);

protected:
	void						Update();

	Shader						waterShader;
	std::unique_ptr<MyModel>	water;
	std::unique_ptr<eObject>	object;

	GLuint						fullTransformationUniformLocation;
	GLuint						modelToWorldMatrixUniformLocation;
	GLuint						WaterFactorLoc;
	GLuint						CameraPosLoc;
	GLuint						lightPosLoc;
	GLuint						lightColorLoc;
	GLuint						clipPlaneLoc;
	
	math::eClock		clock;
	float						 move_factor	= 0.0f;
	float						 waterHeight = 2.0f;
	bool						 incr		= true;
	std::unique_ptr<math::Timer> timer;
	std::unique_ptr<Texture> reflection;
	std::unique_ptr<Texture> refraction;
};
