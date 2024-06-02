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

MyModel::MyModel()
  : m_name("empty")
{}

MyModel::MyModel(std::shared_ptr<MyMesh> m, const std::string& _name, 
  const Texture* t, const Texture* t2, const Texture* t3, const Texture* t4)
  : mesh(m)
  , m_name(_name)
{
  if (t != nullptr)
  {
    m_material.albedo_texture_id = t->m_id;
    m_material.use_albedo = true;
  }
  else
    m_material.albedo_texture_id = Texture::GetTexture1x1(GREY).m_id;
  
  if (t2 != nullptr)
  {
    m_material.metalic_texture_id = t2->m_id;
    m_material.use_metalic = true;
  }
  else
    m_material.metalic_texture_id = Texture::GetTexture1x1(BLACK).m_id;
  if (t3 != nullptr)
  {
    m_material.normal_texture_id = t3->m_id;
    m_material.use_normal = true;
  }
  else
    m_material.normal_texture_id = Texture::GetTexture1x1(BLUE).m_id;

  if (t4 != nullptr)
  {
    m_material.roughness_texture_id = t4->m_id;
    m_material.use_roughness = true;
  }
  else
    m_material.roughness_texture_id = Texture::GetTexture1x1(WHITE).m_id;

  m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).m_id;//!!! add default emissive
}

//-----------------------------------------------------------------
MyModel::MyModel(const MyModel& _other) //shallow copy
	: mesh(_other.mesh)
  , m_material(_other.m_material)
  , m_name(_other.m_name)
  , m_path(_other.m_path)
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

void MyModel::SetMaterial(const Material& _material)
{
  m_material = _material;
  _InitMaterialWithDefaults();
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

void MyModel::_InitMaterialWithDefaults()
{
  if (m_material.albedo_texture_id == -1)
    m_material.albedo_texture_id = Texture::GetTexture1x1(GREY).m_id;

  if (m_material.metalic_texture_id == -1)
    m_material.metalic_texture_id = Texture::GetTexture1x1(BLACK).m_id;

  if (m_material.normal_texture_id == -1)
    m_material.normal_texture_id = Texture::GetTexture1x1(BLUE).m_id;

  if (m_material.roughness_texture_id == -1)
    m_material.roughness_texture_id = Texture::GetTexture1x1(WHITE).m_id;

  if (m_material.emissive_texture_id == -1)
    m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).m_id;
}

size_t MyModel::GetVertexCount() const
{
  return mesh->GetVertexCount();
}

std::vector<const I3DMesh*> MyModel::Get3DMeshes() const
{
  return std::vector<const I3DMesh*> { mesh.get() };
}

std::vector<const IMesh*> MyModel::GetMeshes() const
{
  return std::vector<const IMesh*> { mesh.get() };
}

//---------------------------------------------------------------------------------
SimpleModel::SimpleModel(IMesh* _m)
:m_mesh(_m) {}

SimpleModel::~SimpleModel() { delete m_mesh; }
