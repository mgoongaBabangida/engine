#include "stdafx.h"
#include <base/Object.h>

#include "WaterRender.h"
#include "MyModel.h"
#include "Texture.h"
#include "GlBufferContext.h"

#include <math/Transform.h>

//--------------------------------------------------------------------------------------------------
eWaterRender::eWaterRender(std::unique_ptr<MyModel> model,
													 const Texture*				waves,
													 const Texture*				DUDV,
													 const std::string&		vertexShaderPath,
													 const std::string&		fragmentShaderPath)
{
	waterShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(waterShader.ID(), "modelToWorldMatrix");
	fullTransformationUniformLocation	= glGetUniformLocation(waterShader.ID(), "modelToProjectionMatrix");
	WaterFactorLoc						= glGetUniformLocation(waterShader.ID(), "moveFactor");
	CameraPosLoc						= glGetUniformLocation(waterShader.ID(), "cameraPosition");
	lightPosLoc							= glGetUniformLocation(waterShader.ID(), "lightPosition");
	lightColorLoc						= glGetUniformLocation(waterShader.ID(), "lightColor");

	water_model = model.get();
	model->setTextureBump(waves);
	model->setTextureFourth(DUDV);
	object.reset(new eObject);
	object->SetModel(model.release());
	object->SetTransform(new Transform);
	object->GetTransform()->setTranslation(glm::vec3(0.0f, waterHeight, 0.0f));
	object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);
	object->GetTransform()->setScale(glm::vec3(1.2f, 1.8f, 1.0f)); // the size of the pixture

	clock.start();
}

void eWaterRender::Render(const Camera& camera, const Light& light)
{
	glUseProgram(waterShader.ID());

	glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();

	Texture texture_reflection = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION);
	Texture texture_refraction = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION);
	water_model->setTextureDiffuse(&texture_reflection);
	water_model->setTextureSpecular(&texture_refraction);
	
	glUniform1f(WaterFactorLoc, move_factor);

	int msc = clock.newFrame();
	move_factor += (float)msc / wave_speed_fator;

	glUniform3f(CameraPosLoc,	camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	glUniform3f(lightPosLoc,	light.light_position.x,	light.light_position.y,	light.light_position.z);// , light.light_position.w);
	glUniform3f(lightColorLoc,	light.diffuse.x,		light.diffuse.y,		light.diffuse.z);
	
	glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);
	
	object->GetModel()->Draw();
}

eWaterRender::~eWaterRender()
{
}
