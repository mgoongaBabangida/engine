#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include <memory>

class eObject;
class MyModel;

class eSkyNoiseRender
{
public:
	eSkyNoiseRender(std::unique_ptr<MyModel> model, Texture* noise, const std::string&, const std::string&);
	void Render(const glm::mat4& projectionMatrix, const Camera& camera);

private:
	std::unique_ptr<MyModel> model;
	std::unique_ptr<eObject> object;
	Shader					 skynoise_shader;
	GLuint					 fullTransformationUniformLocation;
};
