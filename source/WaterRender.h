#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Structures.h"
#include "Clock.h"
#include "Timer.h"
#include <memory>

//#include "colorFBO.h"

class eObject;
class MyModel;

class eWaterRender 
{
public:
	eWaterRender(std::unique_ptr<MyModel> model, Texture* waves, Texture* DUDV);
	virtual ~eWaterRender();

	void		Render(const glm::mat4& projectionMatrix, const Camera& camera, const Light& light);
	
	//void		BindReflectionFBO()		{ reflectionFBO->BindForWriting(); }
	//void		BindRefractionFBO()		{ refractionFBO->BindForWriting(); }
	//Texture		GetReflectionTexture()  { return reflectionFBO->GetTexture(); }
	//Texture		GetRefractionTexture()  { return refractionFBO->GetTexture(); }

protected:
	void						Update();

	Shader						waterShader;
	std::unique_ptr<MyModel>	water;
	std::unique_ptr<eObject>	object;

	//eColorFBO* reflectionFBO;
	//eColorFBO* refractionFBO;

	GLuint						fullTransformationUniformLocation;
	GLuint						modelToWorldMatrixUniformLocation;
	GLuint						WaterFactorLoc;
	GLuint						CameraPosLoc;
	GLuint						lightPosLoc;
	GLuint						lightColorLoc;
	GLuint						clipPlaneLoc;
	
	Clock						clock;
	float						move_factor=0.0f;
	bool						incr = true;
	std::unique_ptr<dbb::Timer> timer;
};
