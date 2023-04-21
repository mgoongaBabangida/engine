#include "stdafx.h"
#include "AssimpMesh.h"

#include <sstream>

using namespace std;

Texture AssimpMesh::default_diffuse_mapping = {};
Texture AssimpMesh::default_specular_mapping = {};
Texture AssimpMesh::default_normal_mapping = {};
Texture AssimpMesh::default_emission_mapping = {};

AssimpMesh::AssimpMesh(vector<AssimpVertex> _vertices,
                       vector<GLuint> _indices,
                       vector<Texture> _textures,
                       const Material _material,
                       const std::string& _name)
{
	this->vertices = _vertices;
	this->indices = _indices;
	this->textures = _textures;
  this->material = _material;
  this->name = _name.empty() ? "Default" : _name;
	this->setupMesh();

  if(default_diffuse_mapping.id == GetDefaultTextureId())
    default_diffuse_mapping.loadTexture1x1(GREY);
  if (default_specular_mapping.id == GetDefaultTextureId())
    default_specular_mapping.loadTexture1x1(BLACK);
  if (default_normal_mapping.id == GetDefaultTextureId())
    default_normal_mapping.loadTexture1x1(BLUE);
  if (default_emission_mapping.id == GetDefaultTextureId())
    default_emission_mapping.loadTexture1x1(BLACK);

  if (material.albedo_texture_id == GetDefaultTextureId())
    material.albedo_texture_id = default_diffuse_mapping.id;
  if (material.metalic_texture_id == GetDefaultTextureId())
    material.metalic_texture_id = default_specular_mapping.id;
  if (material.normal_texture_id == GetDefaultTextureId())
    material.normal_texture_id = default_normal_mapping.id;
  if (material.emissive_texture_id == GetDefaultTextureId())
    material.emissive_texture_id = default_emission_mapping.id;
}

AssimpMesh::~AssimpMesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

void AssimpMesh::FreeTextures()
{
  default_diffuse_mapping.freeTexture();
  default_specular_mapping.freeTexture();
  default_normal_mapping.freeTexture();
  default_emission_mapping.freeTexture();

  for (auto& t : textures)
    t.freeTexture();
}

void AssimpMesh::Draw()
{
  //@todo switch to rendering material instead of textures

	GLuint diffuseNr = 0;
	GLuint specularNr = 0;
	GLuint normalNr = 0;
  GLuint emissionNr = 0;
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
    glBindTexture(GL_TEXTURE_2D, default_diffuse_mapping.id);
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
      glBindTexture(GL_TEXTURE_2D, default_specular_mapping.id);
      specularNr++;
    }
  }
  if (normalNr == 0)
  {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, default_normal_mapping.id);
    normalNr++;
  }
  if (emissionNr == 0)
  {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, default_emission_mapping.id);
    emissionNr++;
  }
  if (!diffuseNr || !specularNr || !normalNr || !emissionNr)
  {
    assert(false && "some texture is not assigned!");
  }

  glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AssimpMesh::SetMaterial(const Material& _material)
{
  material = _material;
}

void AssimpMesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(AssimpVertex),
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
		&this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, TexCoords));
	// Vertex Tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, tangent));
	// Vertex Bitangent
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, bitangent));
	// Vertex BoneIDs
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_INT, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, boneIDs));
	// Vertex Weights
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex),
		(GLvoid*)offsetof(AssimpVertex, weights));

	glBindVertexArray(0);
}
