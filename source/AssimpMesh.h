#pragma once
#include <vector>
#include "Texture.h"
#include "Structures.h"

using namespace::std;

class AssimpMesh 
{
public:
	/*  Mesh Data  */
	vector<AssimpVertex>	vertices;
	vector<GLuint>			indices;
	vector<Texture>			textures;

	/*  Functions  */
	AssimpMesh(vector<AssimpVertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	void Draw();

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();
};
