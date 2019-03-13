#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

class eObject;
class MyModel;
class QTimer;

class eSkyNoiseRender
{
public:
	eSkyNoiseRender(MyModel * model, Texture * noise);
	void Render(const glm::mat4& projectionMatrix, const Camera& camera);

private:
	MyModel* m_model;
	eObject* object;
	Shader	 skynoise_shader;
	GLuint   fullTransformationUniformLocation;
};
