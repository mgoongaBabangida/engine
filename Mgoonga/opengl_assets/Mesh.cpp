#include "stdafx.h"

#include "Mesh.h"
#include "GlDrawContext.h"

#include <sstream>

using namespace std;

I3DMesh* MakeMesh(std::vector<Vertex> _vertices,
                  std::vector<GLuint> _indices,
                  std::vector<TextureInfo> _textures,
                  const Material& _material,
                  const std::string& _name ,
                  bool _calculate_tangent)
{
  std::vector<Texture> textures;
  for (auto t : _textures)
    textures.emplace_back(t);

  return new eMesh(_vertices, _indices, textures, _material, _name, _calculate_tangent);
}

//-------------------------------------------------------------------------------------------
eMesh::eMesh(vector<Vertex> _vertices,
                       vector<GLuint> _indices,
                       vector<Texture> _textures,
                       const Material& _material,
                       const std::string& _name,
                       bool _calculate_tangent)
{
	m_vertices = _vertices;
	m_indices = _indices;
	m_textures = _textures;
  m_material = _material;
  m_name = _name.empty() ? "Default" : _name;

	SetupMesh();
  if(_calculate_tangent)
    this->calculatedTangent();

  ReloadTextures();

  if (m_material.albedo_texture_id == Texture::GetDefaultTextureId())
    m_material.albedo_texture_id = Texture::GetTexture1x1(GREY).m_id;
  if (m_material.metalic_texture_id == Texture::GetDefaultTextureId())
    m_material.metalic_texture_id = Texture::GetTexture1x1(BLACK).m_id;
  if (m_material.normal_texture_id == Texture::GetDefaultTextureId())
    m_material.normal_texture_id = Texture::GetTexture1x1(BLUE).m_id;
  if (m_material.roughness_texture_id == Texture::GetDefaultTextureId())
    m_material.roughness_texture_id = Texture::GetTexture1x1(WHITE).m_id;
  if (m_material.emissive_texture_id == Texture::GetDefaultTextureId())
    m_material.emissive_texture_id = Texture::GetTexture1x1(BLACK).m_id;
}

//-------------------------------------------------------------------------------------------
eMesh::~eMesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

//-------------------------------------------------------------------------------------------
void eMesh::ReloadTextures()
{
  for (Texture& t : m_textures)
  {
    if(t.m_id == Texture::GetDefaultTextureId())
      t.loadTextureFromFile(t.m_path);

    if (t.m_type == "texture_diffuse")
      m_material.albedo_texture_id = t.m_id;
    else if (t.m_type == "texture_specular")
      m_material.metalic_texture_id = t.m_id;
    else if (t.m_type == "texture_normal")
      m_material.normal_texture_id = t.m_id;
    else if (t.m_type == "texture_roughness")
      m_material.roughness_texture_id = t.m_id;
    else if (t.m_type == "texture_emission")
      m_material.emissive_texture_id = t.m_id;
  }
}

//-------------------------------------------------------------------------------------------
void eMesh::FreeTextures()
{
  for (auto& t : m_textures)
    t.freeTexture();
}

//-------------------------------------------------------------------------------------------
void eMesh::ReloadVertexBuffer()
{
  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex),
    &this->m_vertices[0], GL_STATIC_DRAW);
  glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------
void eMesh::Draw()
{
  _BindMaterialTextures(); // _BindRawTextures();
	// Draw mesh
	glBindVertexArray(this->VAO);
  eGlDrawContext::GetInstance().DrawElements(GL_TRIANGLES, (GLsizei)this->m_indices.size(), GL_UNSIGNED_INT, 0, this->m_name);
	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------
void eMesh::DrawInstanced(int32_t instances)
{
  _BindMaterialTextures(); // _BindRawTextures();
  // Draw mesh
  glBindVertexArray(this->VAO);
  eGlDrawContext::GetInstance().DrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)(0), instances, this->m_name);
  glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------
std::vector<TextureInfo> eMesh::GetTextures() const
{
   std::vector<TextureInfo> ret;
   for (auto& t : m_textures)
     ret.emplace_back(t.m_type, t.m_path);
   return ret;
}

//-------------------------------------------------------------------------------------------
void eMesh::AddTexture(Texture* _texture)
{
  if (_texture->m_type == "texture_diffuse")
    m_material.albedo_texture_id = _texture->m_id;
  else if (_texture->m_type == "texture_specular")
    m_material.metalic_texture_id = _texture->m_id;
  else if (_texture->m_type == "texture_normal")
    m_material.normal_texture_id = _texture->m_id;
  else if (_texture->m_type == "texture_roughness")
    m_material.roughness_texture_id = _texture->m_id;
  else if (_texture->m_type == "texture_emission")
    m_material.emissive_texture_id = _texture->m_id;

  m_textures.push_back(*_texture);
}

//-------------------------------------------------------------------------------------------
void eMesh::SetMaterial(const Material& _material)
{
  m_material = _material;
}

//-------------------------------------------------------------------------------------------
void eMesh::calculatedTangent()
{
  for (int i = 0; i < m_indices.size(); i += 3)
  {
    glm::vec3 pos1 = m_vertices[m_indices[i]].Position;
    glm::vec3 pos2 = m_vertices[m_indices[i + 1]].Position;
    glm::vec3 pos3 = m_vertices[m_indices[i + 2]].Position;
    glm::vec2 uv1 = m_vertices[m_indices[i]].TexCoords;
    glm::vec2 uv2 = m_vertices[m_indices[i + 1]].TexCoords;
    glm::vec2 uv3 = m_vertices[m_indices[i + 2]].TexCoords;
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

    m_vertices[m_indices[i]].tangent = tangent1;
    m_vertices[m_indices[i + 1]].tangent = tangent1;
    m_vertices[m_indices[i + 2]].tangent = tangent1;
    m_vertices[m_indices[i]].bitangent = bitangent1;
    m_vertices[m_indices[i + 1]].bitangent = bitangent1;
    m_vertices[m_indices[i + 2]].bitangent = bitangent1;
  }
}

//-------------------------------------------------------------------------------------------
void eMesh::_BindRawTextures()
{
  GLuint diffuseNr = 0;
  GLuint specularNr = 0;
  GLuint normalNr = 0;
  GLuint emissionNr = 0;
  GLuint roughnesslNr = 0;
  for (GLuint i = 0; i < this->m_textures.size(); i++)
  {
    string name = m_textures[i].m_type;
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
    glBindTexture(GL_TEXTURE_2D, this->m_textures[i].m_id);
  }

  if (diffuseNr == 0)
  {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(GREY).m_id);
    diffuseNr++;
  }
  if (specularNr == 0)
  {
    glActiveTexture(GL_TEXTURE3);
    auto spec = std::find_if(m_textures.begin(), m_textures.end(), [](Texture& t) { return t.m_type == "texture_diffuse"; });
    if (spec != m_textures.end())
    {
      glBindTexture(GL_TEXTURE_2D, spec->m_id);
      specularNr++;
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLACK).m_id);
      specularNr++;
    }
  }
  if (normalNr == 0)
  {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLUE).m_id);
    normalNr++;
  }
  if (roughnesslNr == 0)
  {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(WHITE).m_id);
    roughnesslNr++;
  }
  if (emissionNr == 0)
  {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(BLACK).m_id);
    emissionNr++;
  }
  if (!diffuseNr || !specularNr || !normalNr || !emissionNr)
  {
    assert(false && "some texture is not assigned!");
  }

  glActiveTexture(GL_TEXTURE0);
}

//-------------------------------------------------------------------------------------------
void eMesh::_BindMaterialTextures()
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

//-------------------------------------------------------------------------------------------
void eMesh::SetupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex),
		&this->m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(GLuint),
		&this->m_indices[0], GL_STATIC_DRAW);

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
