#include "stdafx.h"
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

MyModel::MyModel()
{
}

MyModel::MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3, Texture* t4)
  : mesh(m)
  , m_diffuse(t)
  , m_specular(t2)
  , m_bump(t3)
  , m_fourth(t4)
{
  if (m_specular == nullptr)
    m_specular = m_diffuse;
  if (m_bump == nullptr)
    m_bump = m_diffuse;
  if (m_fourth == nullptr)
    m_fourth = m_diffuse;
}

MyModel::MyModel(const MyModel& _other) //shallow copy
	: mesh(_other.mesh), m_diffuse(_other.m_diffuse), m_specular(_other.m_specular), m_bump(_other.m_bump), m_fourth(_other.m_fourth)
{
}

MyModel::~MyModel()
{

}

std::vector<MyMesh*> MyModel::getMeshes() const 
{
  std::vector<MyMesh*> meshes;
  meshes.push_back(mesh.get());
  return meshes;
}

void MyModel::SetTexture(Texture* t) 
{ 
  m_diffuse = t; 
  m_specular = t; 
  m_bump = t;
}

void MyModel::setTextureDiffuse(Texture* t)
{ 
  m_diffuse = t;
}
void MyModel::setTextureSpecular(Texture* t) 
{ 
  m_specular = t;
}
void MyModel::setTextureBump(Texture* t) 
{ 
  m_bump = t;
}
void MyModel::setTextureFourth(Texture* t) 
{ 
  m_fourth = t;
}

std::vector<const Texture*> MyModel::GetTexturesModelLevel() const
{
  return std::vector<const Texture*> { m_diffuse , m_specular , m_bump , m_fourth };
}

void MyModel::Draw()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_diffuse->id);
	//glUniform1i(glGetUniformLocation(Program, "texture_diffuse1"), 2); other way to do it, may be useful

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_specular->id);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_bump->id);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_fourth->id);

	mesh->Draw();
}

void MyModel::Debug()
{

}

std::vector<glm::vec3> MyModel::GetPositions() const
{
	std::vector<glm::vec3> ret;
	for (auto& vert : mesh->vertices)
		ret.push_back(vert.position);
	return ret;
}

std::vector<GLuint> MyModel::GetIndeces() const
{
	return mesh->indices;
}

size_t MyModel::GetVertexCount() const
{
  return mesh->GetVertexCount();
}

std::vector<const IMesh*> MyModel::GetMeshes() const
{
  return std::vector<const IMesh*> { mesh.get() };
}
