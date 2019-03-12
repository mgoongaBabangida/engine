#include "MyModel.h"
#include "Texture.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm/gtx/euler_angles.hpp>
#include <glm\glm/gtc/quaternion.hpp>
#include <glm\glm/gtx/quaternion.hpp>
#include <glm\glm/gtx/norm.hpp>

#include <iostream>

void MyModel::Draw()
{
	//int loc = glGetUniformLocation(Program, "gBones");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, &glm::mat4()[0][0]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_diffuse->id);
	//glUniform1i(glGetUniformLocation(Program, "texture_diffuse1"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_specular->id);
	//glUniform1i(glGetUniformLocation(Program, "texture_specular1"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_bump->id);
	//glUniform1i(glGetUniformLocation(Program, "texture_normal1"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_fourth->id);
	//glUniform1i(glGetUniformLocation(Program, "texture_fourth1"), 5);

	mesh->Draw();
}

void MyModel::Debug()
{

}

std::vector<glm::vec3> MyModel::getPositions() const 
{
	std::vector<glm::vec3> ret;
	for (auto& vert : mesh->vertices)
		ret.push_back(vert.position);
	return ret;
}

std::vector<GLuint> MyModel::getIndeces() const
{
	return mesh->indices; 
}
