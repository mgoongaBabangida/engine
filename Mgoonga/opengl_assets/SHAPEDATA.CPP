#include "stdafx.h"
#include "ShapeData.h"

ShapeData::ShapeData()
: vertices(nullptr), numVertices(0), indices(nullptr), numIndices(0) 
{}

GLsizeiptr ShapeData::vertexBufferSize() const
{
	return numIndices * sizeof(Vertex);
}

GLsizeiptr ShapeData::indexBufferSize() const
{
	return numIndices * sizeof(indices);
}

void ShapeData::cleanup()
{
	delete[] vertices;
	delete[] indices;
	numVertices = numIndices = 0;
}
