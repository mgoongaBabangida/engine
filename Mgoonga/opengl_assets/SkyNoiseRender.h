#pragma once
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <memory>

class eObject;
class MyModel;

class eSkyNoiseRender
{
public:
	eSkyNoiseRender(std::unique_ptr<MyModel> model, Texture* noise, const std::string&, const std::string&);
	void Render(const Camera& camera);

private:
	std::unique_ptr<MyModel> model;
	std::unique_ptr<eObject> object;

	Shader					 skynoise_shader;

	GLuint					 fullTransformationUniformLocation;
	GLuint					 moveFactorLocation;
	float					   skyHegight = 5.0f;
	float				     moveFactor = 0.0f;
};
