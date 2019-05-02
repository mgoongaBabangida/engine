#pragma once

#include "Vertex.h"

struct ShapeData
{
	ShapeData() :vertices(nullptr), numVertices(0), indices(nullptr), numIndices(0) {};
	Vertex* vertices;
	GLuint numVertices;
	GLushort* indices;
	GLuint numIndices;
	GLsizeiptr vertexBufferSize() const
	{
		return numIndices * sizeof(Vertex);
	}
	GLsizeiptr indexBufferSize() const
	{
		return numIndices * sizeof(indices);
	}

	void cleanup()
	{
		delete[] vertices;
		delete[] indices;
		numVertices = numIndices = 0;
	}	
};
