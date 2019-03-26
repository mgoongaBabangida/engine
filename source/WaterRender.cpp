#include "stdafx.h"
#include "WaterRender.h"
#include "Object.h"
#include "MyModel.h"
#include "GlBufferContext.h"

eWaterRender::eWaterRender(std::unique_ptr<MyModel> model, 
							Texture*				waves, 
							Texture*				DUDV, 
							const string&			vertexShaderPath,
							const string&			fragmentShaderPath)
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

	object.reset(new eObject(water.get()));
	object->getTransform()->setTranslation(glm::vec3(0.0f, 2.0f, 0.0f));  // HEIGHT!
	object->getTransform()->setRotation(PI / 2, 0.0f, 0.0f);

	clock.start();
	timer.reset(new dbb::Timer([this]()->bool {this->Update(); return true; }));
	timer->start(100);
}

void eWaterRender::Render(const glm::mat4& ProjectionMatrix, const Camera& camera, const Light& light)
{
	glUseProgram(waterShader.ID);

	mat4 worldToProjectionMatrix = ProjectionMatrix * camera.getWorldToViewMatrix();
	water->setTextureDiffuse(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
	water->setTextureSpecular(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION));
	
	//move factor (has to be improved)
	glUniform1f(WaterFactorLoc, move_factor);

	int msc = clock.newFrame();
	//if(incr)
	move_factor += (float)msc / 10000.0f;
	//else
	//this->move_factor -= (float)msc / 10000.0f;

	if (move_factor > 0.05f)
		move_factor = -0.05f;

	glUniform3f(CameraPosLoc,	camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	glUniform3f(lightPosLoc,	light.light_vector.x,	light.light_vector.y,	light.light_vector.z);// , light.light_vector.w);
	glUniform3f(lightColorLoc,	light.diffuse.x,		light.diffuse.y,		light.diffuse.z);
	
	mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->getTransform()->getModelMatrix()[0][0]);
	
	object->getModel()->Draw();
}

void eWaterRender::Update(){}

eWaterRender::~eWaterRender()
{}
