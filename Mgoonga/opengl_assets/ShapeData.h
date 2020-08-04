#pragma once

#include <glew-2.1.0\include\GL\glew.h>

#include "glm\glm\glm.hpp"
#include "opengl_assets.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

struct ShapeData
{
	ShapeData();
	Vertex*		vertices;
	GLuint		numVertices;
	GLushort*	indices;
	GLuint		numIndices;
	
	GLsizeiptr vertexBufferSize() const;
	GLsizeiptr indexBufferSize() const;

	void cleanup();
};
