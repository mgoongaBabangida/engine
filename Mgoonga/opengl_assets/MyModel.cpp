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

Texture MyModel::default_diffuse_mapping = {};
Texture MyModel::default_specular_mapping = {};
Texture MyModel::default_normal_mapping = {};
Texture MyModel::default_emission_mapping = {};

MyModel::MyModel()
{}

MyModel::MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3, Texture* t4)
  : mesh(m)
{
  if (default_diffuse_mapping.id == GetDefaultTextureId())
    default_diffuse_mapping.loadTexture1x1(GREY);
  if (default_specular_mapping.id == GetDefaultTextureId())
    default_specular_mapping.loadTexture1x1(BLACK);
  if (default_normal_mapping.id == GetDefaultTextureId())
    default_normal_mapping.loadTexture1x1(BLUE);
  if (default_emission_mapping.id == GetDefaultTextureId())
    default_emission_mapping.loadTexture1x1(BLACK);

  if (t != nullptr)
  {
    m_material.albedo_texture_id = t->id;
    m_material.use_albedo = true;
  }
  else
    m_material.albedo_texture_id = default_diffuse_mapping.id;
  
  if (t2 != nullptr)
  {
    m_material.metalic_texture_id = t2->id;
    m_material.use_metalic = true;
  }
  else
    m_material.metalic_texture_id = default_specular_mapping.id;

  if (t3 != nullptr)
  {
    m_material.normal_texture_id = t3->id;
    m_material.use_normal = true;
  }
  else
    m_material.normal_texture_id = default_normal_mapping.id;

  if (t4 != nullptr)
  {
    m_material.roughness_texture_id = t4->id;
    m_material.use_roughness = true;
  }
  else
    m_material.roughness_texture_id = default_emission_mapping.id; //!!! add default emissive

  m_material.emissive_texture_id = default_emission_mapping.id;
}

MyModel::MyModel(const MyModel& _other) //shallow copy
	: mesh(_other.mesh)
  , m_material(_other.m_material)
{
}

MyModel::~MyModel()
{}

std::vector<MyMesh*> MyModel::getMeshes() const 
{
  std::vector<MyMesh*> meshes;
  meshes.push_back(mesh.get());
  return meshes;
}

void MyModel::SetTexture(Texture* t) 
{ 
  m_material.albedo_texture_id = t->id;
  m_material.metalic_texture_id = t->id;
  m_material.normal_texture_id = t->id;
}

void MyModel::setTextureDiffuse(Texture* t)
{ 
  m_material.albedo_texture_id = t->id;
}
void MyModel::setTextureSpecular(Texture* t)
{ 
  m_material.metalic_texture_id = t->id;
}
void MyModel::setTextureBump(Texture* t)
{ 
  m_material.normal_texture_id = t->id;
}
void MyModel::setTextureFourth(Texture* t)
{ 
  m_material.roughness_texture_id = t->id;
}

void MyModel::Draw()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_material.albedo_texture_id);
	//glUniform1i(glGetUniformLocation(Program, "texture_diffuse1"), 2); other way to do it, may be useful

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_material.metalic_texture_id);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_material.normal_texture_id);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_material.roughness_texture_id);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_material.emissive_texture_id);

	mesh->Draw();
}

void MyModel::Debug()
{}

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
