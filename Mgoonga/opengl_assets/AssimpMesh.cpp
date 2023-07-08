#include "stdafx.h"
#include "AssimpMesh.h"

#include <sstream>

using namespace std;

AssimpMesh::AssimpMesh(vector<Vertex> _vertices,
                       vector<GLuint> _indices,
                       vector<Texture> _textures,
                       const Material _material,
                       const std::string& _name,
                       bool _calculate_tangent)
{
	this->vertices = _vertices;
	this->indices = _indices;
	this->textures = _textures;
  this->m_material = _material;
  this->name = _name.empty() ? "Default" : _name;
	this->setupMesh();
  if(_calculate_tangent)
    this->calculatedTangent();

  if (m_material.albedo_texture_id == Texture::GetDefaultTextureId())
    m_material.albedo_texture_id = Texture::GetTexture1x1(GREY).id;
  if (m_material.metalic_texture_id == Texture::GetDefaultTextureId())
    m_material.metalic_texture_id = Texture::GetTexture1x1(BLACK).id;
  if (m_material.normal_texture_id == Texture::GetDefaultTextureId())
    m_material.normal_texture_id = Texture::GetTexture1x1(BLUE).id;
  if (m_material.roughness_texture_id == Texture::GetDefaultTextureId())
    m_material.roughness_texture_id = Texture::GetTexture1x1(WHITE).id;
  if (m_material.emissive_texture_id == Texture::GetDefaultTextureId())
    m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
}

AssimpMesh::~AssimpMesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

void AssimpMesh::FreeTextures()
{
  for (auto& t : textures)
    t.freeTexture();
}

void AssimpMesh::Draw()
{
  _BindMaterialTextures(); // _BindRawTextures();
	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AssimpMesh::DrawInstanced(int32_t instances)
{
  _BindMaterialTextures(); // _BindRawTextures();
  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, (GLvoid*)(0), instances);
  glBindVertexArray(0);
}

std::vector<TextureInfo> AssimpMesh::GetTextures() const
{
   std::vector<TextureInfo> ret;
   for (auto& t : textures)
     ret.emplace_back(t.type, t.path);
   return ret;
}

void AssimpMesh::AddTexture(Texture* _texture)
{
  if (_texture->type == "texture_diffuse")
    m_material.albedo_texture_id = _texture->id;
  else if (_texture->type == "texture_specular")
    m_material.metalic_texture_id = _texture->id;
  else if (_texture->type == "texture_normal")
    m_material.normal_texture_id = _texture->id;
  else if (_texture->type == "texture_roughness")
    m_material.roughness_texture_id = _texture->id;
  else if (_texture->type == "texture_emission")
    m_material.emissive_texture_id = _texture->id;

  textures.push_back(*_texture);
}

void AssimpMesh::SetMaterial(const Material& _material)
{
  m_material = _material;
}

void AssimpMesh::calculatedTangent()
{
  for (int i = 0; i < indices.size(); i += 3)
  {
    glm::vec3 pos1 = vertices[indices[i]].Position;
    glm::vec3 pos2 = vertices[indices[i + 1]].Position;
    glm::vec3 pos3 = vertices[indices[i + 2]].Position;
    glm::vec2 uv1 = vertices[indices[i]].TexCoords;
    glm::vec2 uv2 = vertices[indices[i + 1]].TexCoords;
    glm::vec2 uv3 = vertices[indices[i + 2]].TexCoords;
    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    // - triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    vertices[indices[i]].tangent = tangent1;
    vertices[indices[i + 1]].tangent = tangent1;
    vertices[indices[i + 2]].tangent = tangent1;
    vertices[indices[i]].bitangent = bitangent1;
    vertices[indices[i + 1]].bitangent = bitangent1;
    vertices[indices[i + 2]].bitangent = bitangent1;
  }
}

void AssimpMesh::_BindRawTextures()
{
  GLuint diffuseNr = 0;
  GLuint specularNr = 0;
  GLuint normalNr = 0;
  GLuint emissionNr = 0;
  GLuint roughnesslNr = 0;
  for (GLuint i = 0; i < this->textures.size(); i++)
  {
    string name = textures[i].type;
    if (name == "texture_diffuse")
    {
      glActiveTexture(GL_TEXTURE2);
      diffuseNr++;
    }
    else if (name == "texture_specular")
    {
      glActiveTexture(GL_TEXTURE3);
      specularNr++;
    }
    else if (name == "texture_normal")
    {
      glActiveTexture(GL_TEXTURE4);
      normalNr++;
    }
    else if (name == "texture_roughness")
    {
      glActiveTexture(GL_TEXTURE5);
      roughnesslNr++;
    }
    else if (name == "texture_emission")
    {
      glActiveTexture(GL_TEXTURE6);
      emissionNr++;
    }
    glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
  }

  if (diffuseNr == 0)
  {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(GREY).id);
    diffuseNr++;
  }
  if (specularNr == 0)
  {
    glActiveTexture(GL_TEXTURE3);
    auto spec = std::find_if(textures.begin(), textures.end(), [](Texture& t) { return t.type == "texture_diffuse"; });
    if (spec != textures.end())
    {
      glBindTexture(GL_TEXTURE_2D, spec->id);
      specularNr++;
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLACK).id);
      specularNr++;
    }
  }
  if (normalNr == 0)
  {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLUE).id);
    normalNr++;
  }
  if (roughnesslNr == 0)
  {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(WHITE).id);
    roughnesslNr++;
  }
  if (emissionNr == 0)
  {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLACK).id);
    emissionNr++;
  }
  if (!diffuseNr || !specularNr || !normalNr || !emissionNr)
  {
    assert(false && "some texture is not assigned!");
  }

  glActiveTexture(GL_TEXTURE0);
}

void AssimpMesh::_BindMaterialTextures()
{
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_material.albedo_texture_id);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_material.metalic_texture_id);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, m_material.normal_texture_id);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, m_material.roughness_texture_id);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_material.emissive_texture_id);
}

void AssimpMesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
		&this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, TexCoords));
	// Vertex Tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, tangent));
	// Vertex Bitangent
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, bitangent));
	// Vertex BoneIDs
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, boneIDs));
	// Vertex Weights
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, weights));

	glBindVertexArray(0);
}
