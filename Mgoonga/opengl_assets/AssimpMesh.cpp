#include "stdafx.h"
#include "AssimpMesh.h"

#include <sstream>
#include <iostream>

AssimpMesh::AssimpMesh(vector<AssimpVertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->setupMesh();
  default_diffuse_mapping.loadTexture1x1(YELLOW);
  default_specular_mapping.loadTexture1x1(BLACK);
  default_normal_mapping.loadTexture1x1(BLUE);
}

void AssimpMesh::Draw()
{
	GLuint diffuseNr = 0;
	GLuint specularNr = 0;
	GLuint normalNr = 0;
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
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

  if (diffuseNr == 0)
  {
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

  if (!diffuseNr || !specularNr || !normalNr)
  {
    assert(false, "some texture is not assigned!");
  }

  glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<const Texture*> AssimpMesh::GetTextures() const
{
  std::vector<const Texture*> ret;
  for (const Texture& t : textures)
    ret.push_back(&t);
  return ret;
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
