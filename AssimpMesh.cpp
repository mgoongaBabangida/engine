#include "stdafx.h"
#include "AssimpMesh.h"
#include <sstream>
#include <iostream>

AssimpMesh::AssimpMesh(vector<AssimpVertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	//for (int i = 0; i < indices.size(); i += 3)
	//{
	//	glm::vec3 pos1 = vertices[indices[i]].Position;
	//	glm::vec3 pos2 = vertices[indices[i + 1]].Position;
	//	glm::vec3 pos3 = vertices[indices[i + 2]].Position;
	//	glm::vec2 uv1 = vertices[indices[i]].TexCoords;
	//	glm::vec2 uv2 = vertices[indices[i + 1]].TexCoords;
	//	glm::vec2 uv3 = vertices[indices[i + 2]].TexCoords;
	//	// calculate tangent/bitangent vectors of both triangles
	//	glm::vec3 tangent1, bitangent1;
	//	// - triangle 1
	//	glm::vec3 edge1 = pos2 - pos1;
	//	glm::vec3 edge2 = pos3 - pos1;
	//	glm::vec2 deltaUV1 = uv2 - uv1;
	//	glm::vec2 deltaUV2 = uv3 - uv1;

	//	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	//	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	//	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	//	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	//	tangent1 = glm::normalize(tangent1);

	//	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	//	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	//	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	//	bitangent1 = glm::normalize(bitangent1);

	//	vertices[indices[i]].tangent = tangent1;
	//	vertices[indices[i + 1]].tangent = tangent1;
	//	vertices[indices[i + 2]].tangent = tangent1;
	//	vertices[indices[i]].bitangent = bitangent1;
	//	vertices[indices[i + 1]].bitangent = bitangent1;
	//	vertices[indices[i + 2]].bitangent = bitangent1;
	//}

	this->setupMesh();

}

void AssimpMesh::Draw()
{
	//std::cout << "-----------Mesh-----------" << std::endl;
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE2 + i); // Activate proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		else if(name == "texture_normal")
			ss << normalNr++;
		number = ss.str();
		//std::cout << (name + number).c_str() << " bound to texture unit " << i + 2 <<std::endl;
		//glUniform1f(glGetUniformLocation(Program, (/*"material." +*/ name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
