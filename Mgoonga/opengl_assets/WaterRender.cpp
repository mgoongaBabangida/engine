#include "stdafx.h"
#include <base/Object.h>

#include "WaterRender.h"
#include "MyModel.h"
#include "GlBufferContext.h"
#include <math/Transform.h>

//--------------------------------------------------------------------------------------------------
eWaterRender::eWaterRender(std::unique_ptr<MyModel> model, 
							Texture*				waves, 
							Texture*				DUDV, 
							const std::string&		vertexShaderPath,
							const std::string&		fragmentShaderPath)
{
	waterShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	
	modelToWorldMatrixUniformLocation	= glGetUniformLocation(waterShader.ID, "modelToWorldMatrix");
	fullTransformationUniformLocation	= glGetUniformLocation(waterShader.ID, "modelToProjectionMatrix");
	WaterFactorLoc						= glGetUniformLocation(waterShader.ID, "moveFactor");
	CameraPosLoc						= glGetUniformLocation(waterShader.ID, "cameraPosition");
	lightPosLoc							= glGetUniformLocation(waterShader.ID, "lightPosition");
	lightColorLoc						= glGetUniformLocation(waterShader.ID, "lightColor");

	water.swap(model);
	water->setTextureBump(waves);
	water->setTextureFourth(DUDV);
	//@todo object not used , model & transform
	object.reset(new eObject);
	object->SetModel(water.get());
	object->SetTransform(new Transform);
	object->GetTransform()->setTranslation(glm::vec3(0.0f, waterHeight, 0.0f));
	object->GetTransform()->setRotation(PI / 2, 0.0f, 0.0f);
	object->GetTransform()->setScale(glm::vec3(1.18f, 1.8f, 1.0f)); // the size of the pixture

	clock.start();
	timer.reset(new math::Timer([this]()->bool {this->Update(); return true; }));
	timer->start(100);
}

void eWaterRender::Render(const Camera& camera, const Light& light)
{
	glUseProgram(waterShader.ID);

	glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	water->setTextureDiffuse(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
	water->setTextureSpecular(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION));
	
	//move factor (has to be improved)
	glUniform1f(WaterFactorLoc, move_factor);

	int msc = clock.newFrame();
	move_factor += (float)msc / 10000.0f;

	if (move_factor > 0.05f)
		move_factor = -0.05f;

	glUniform3f(CameraPosLoc,	camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	glUniform3f(lightPosLoc,	light.light_position.x,	light.light_position.y,	light.light_position.z);// , light.light_position.w);
	glUniform3f(lightColorLoc,	light.diffuse.x,		light.diffuse.y,		light.diffuse.z);
	
	glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);
	
	object->GetModel()->Draw();
}

void eWaterRender::Update(){}

eWaterRender::~eWaterRender()
{}
