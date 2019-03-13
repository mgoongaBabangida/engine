#include "stdafx.h"
#include "SkyNoiseRender.h"
#include "Object.h"
#include "MyModel.h"
#include "Structures.h"

eSkyNoiseRender::eSkyNoiseRender(MyModel* model, Texture* noise)
{
	skynoise_shader.installShaders("SkyNoiseVertexShader.glsl", "SkyNoiseFragmentShader.glsl");

	fullTransformationUniformLocation = glGetUniformLocation(skynoise_shader.ID, "modelToProjectionMatrix");

	m_model = model;
	m_model->setTextureBump(noise);
	m_model->setTextureFourth(noise);
	object = new eObject(m_model);
	object->getTransform()->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));  // HEIGHT!
	object->getTransform()->setRotation(str::PI / 2, 0.0f, 0.0f);
}

void eSkyNoiseRender::Render(const glm::mat4& ProjectionMatrix, const Camera& camera)
{
	glUseProgram(this->skynoise_shader.ID);
	mat4 worldToProjectionMatrix = ProjectionMatrix * camera.getWorldToViewMatrix();
	mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	object->getModel()->Draw();
}
