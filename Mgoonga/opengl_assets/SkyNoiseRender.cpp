#include "stdafx.h"
#include "SkyNoiseRender.h"
#include "MyModel.h"
#include <base/base.h>
#include <base/Object.h>
#include <math/Transform.h>

//------------------------------------------------------------------------------------------------------
eSkyNoiseRender::eSkyNoiseRender(std::unique_ptr<MyModel>	_model, 
								 Texture*					_noise, 
								 const std::string&			_vS, 
								 const std::string&			_fS)
{
	skynoise_shader.installShaders(_vS.c_str(), _fS.c_str());

	fullTransformationUniformLocation = glGetUniformLocation(skynoise_shader.ID, "modelToProjectionMatrix");
	moveFactorLocation = glGetUniformLocation(skynoise_shader.ID, "moveFactor");
	//@todo object not used , model & transform
	model.swap(_model);
	model->setTextureBump(_noise);
	model->setTextureFourth(_noise);
	object.reset(new eObject);
	object->SetModel(model.get());
	object->SetTransform(new Transform);
	object->GetTransform()->setTranslation(glm::vec3(0.0f, skyHegight, 0.0f));
	object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);
	object->GetTransform()->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
}

//---------------------------------------------------------------------------------------
void eSkyNoiseRender::Render(const Camera& camera)
{
	glUseProgram(this->skynoise_shader.ID);
	//mat4 worldToProjectionMatrix = ProjectionMatrix * camera.getWorldToViewMatrix();
	//mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &UNIT_MATRIX[0][0]); //&modelToProjectionMatrix[0][0]
	//moveFactor += 0.005f;
	glUniform1f(moveFactorLocation, moveFactor);
	object->GetModel()->Draw();
}
