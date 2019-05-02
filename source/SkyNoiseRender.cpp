#include "stdafx.h"
#include "SkyNoiseRender.h"
#include "Object.h"
#include "MyModel.h"
#include "Structures.h"

eSkyNoiseRender::eSkyNoiseRender(std::unique_ptr<MyModel>	_model, 
								 Texture*					_noise, 
								 const std::string&			_vS, 
								 const std::string&			_fS)
{
	skynoise_shader.installShaders(_vS.c_str(), _fS.c_str());

	fullTransformationUniformLocation = glGetUniformLocation(skynoise_shader.ID, "modelToProjectionMatrix");

	model.swap(_model);
	model->setTextureBump(_noise);
	model->setTextureFourth(_noise);
	object.reset(new eObject(model.get()));
	object->getTransform()->setTranslation(glm::vec3(0.0f, 5.0f, 0.0f));  // HEIGHT!
	object->getTransform()->setRotation(PI / 2, 0.0f, 0.0f);
}

void eSkyNoiseRender::Render(const glm::mat4& ProjectionMatrix, const Camera& camera)
{
	glUseProgram(this->skynoise_shader.ID);
	mat4 worldToProjectionMatrix = ProjectionMatrix * camera.getWorldToViewMatrix();
	mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	object->getModel()->Draw();
}
