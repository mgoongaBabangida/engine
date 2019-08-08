#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"
#include "Clock.h"
#include "Timer.h"
#include <memory>

class MyModel;
class eObject;

class eWaterRender 
{
public:
	eWaterRender(std::unique_ptr<MyModel> model, 
				Texture* waves, 
				Texture* DUDV,
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath);
	virtual ~eWaterRender();

	void						Render(const glm::mat4& projectionMatrix, 
									   const Camera&	camera, 
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
	
	eClock						clock;
	float						move_factor	= 0.0f;
	float						waterHeight = 2.0f;
	bool						incr		= true;
	std::unique_ptr<dbb::Timer> timer;
};
