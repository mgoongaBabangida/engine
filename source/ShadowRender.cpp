#include "stdafx.h"
#include "ShadowRender.h"

eShadowRender::eShadowRender()
{
	shader.installShaders("VertexShades.glsl", "FragmentShades.glsl");  //shadows

	MVPUniformLocation = glGetUniformLocation(shader.ID, "MVP");
}

void eShadowRender::Render(const glm::mat4&			projectionMatrix, 
						  const Camera&				camera,
						  const Light&				light, 
						  std::vector<shObject>&	objects)
{
	glUseProgram(shader.ID);

	glm::mat4 ModelToWorldMatrix = glm::translate(glm::vec3(light.light_vector));
	GLfloat near_plane = 0.1f, far_plane = 20.0f; //!!!!!!!!!!!!!!!!!!!!!!!!!!
	//mat4 viewToProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	
	glm::mat4 viewToProjectionMatrix = projectionMatrix;
	glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(light.light_vector), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix; //matrix for shading
	glm::mat4 MVP = viewToProjectionMatrix * worldToViewMatrix * ModelToWorldMatrix;
	
	glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, &MVP[0][0]);


	//RENDER DEPTH
	for (auto &object : objects) 
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->getTransform()->getModelMatrix();
		glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
		//*********************
		std::vector<glm::mat4> matrices(100);
		if (object->getRigger() != nullptr)
		{
			matrices = object->getRigger()->GetMatrices();
		}
		int loc = glGetUniformLocation(shader.ID, "gBones");
		glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);
		//*********************
		object->getModel()->Draw();
	}
}
