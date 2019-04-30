#include "stdafx.h"
#include "DefferedRender.h"
#include "GlBufferContext.h"

eDefferedRender::eDefferedRender(const std::string & vS, const std::string & fS, const std::string & vSS, const std::string & fSS)
{
	gShader.installShaders(vS.c_str(), fS.c_str());

	fullTransformationUniformLocation = glGetUniformLocation(gShader.ID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(gShader.ID, "modelToWorldMatrix");
	eyePositionWorldUniformLocation = glGetUniformLocation(gShader.ID, "eyePositionWorld");

	screenShader.installShaders(vSS.c_str(), fSS.c_str());

	viewPosUniformLocation = glGetUniformLocation(screenShader.ID, "viewPos");
	
	// lighting info
	// -------------
	const unsigned int NR_LIGHTS = 32;
	lightPositions;
	lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
}

void eDefferedRender::Render(const glm::mat4 & projectionMatrix, const Camera & camera, std::vector<shObject>& objects)
{
	glUseProgram(gShader.ID);

	glm::mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	glm::mat4 worldToProjectionMatrix = projectionMatrix * worldToViewMatrix;

	glm::vec3 eyePosition = camera.getPosition();
	glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePosition[0]);

	for (auto &object : objects)
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
		glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->getTransform()->getModelMatrix()[0][0]);
		//*********************
		std::vector<glm::mat4> matrices(100);
		for (auto&m : matrices)
		{
			m = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		}
		if (object->getRigger() != nullptr)
		{
			matrices = object->getRigger()->GetMatrices();
		}
		int loc = glGetUniformLocation(gShader.ID, "gBones");
		glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);
		//*********************
		object->getModel()->Draw();
	}
}

void eDefferedRender::RenderScreen(const Camera& camera)
{
	glUseProgram(screenShader.ID);

	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE1);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED1, GL_TEXTURE2);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED2, GL_TEXTURE3);
	
	// send light relevant uniforms
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		glUniform3f(glGetUniformLocation(screenShader.ID, (const GLchar*)std::string("lights[" + std::to_string(i) + "].Position").c_str()), 1, GL_FALSE, lightPositions[i][0]);
		glUniform3f(glGetUniformLocation(screenShader.ID, (const GLchar*)std::string("lights[" + std::to_string(i) + "].Color").c_str()), 1, GL_FALSE, lightColors[i][0]);
		// update attenuation parameters and calculate radius
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.7;
		const float quadratic = 1.8;
		glUniform1f(glGetUniformLocation(screenShader.ID, (const GLchar*)std::string("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(screenShader.ID, (const GLchar*)std::string("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		glUniform1f(glGetUniformLocation(screenShader.ID, (const GLchar*)std::string("lights[" + std::to_string(i) + "].Radius").c_str()), radius);
	}
	glUniform3fv(viewPosUniformLocation, 1, &camera.getPosition()[0]);
	// finally render quad
	screenMesh->Draw();
}
