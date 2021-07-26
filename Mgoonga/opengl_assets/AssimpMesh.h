#pragma once

#include <vector>
#include "Texture.h"
#include <base/interfaces.h>

using namespace::std;

class AssimpMesh : public IMesh
{
public:
	/*  Functions  */
	AssimpMesh(vector<AssimpVertex> vertices, vector<GLuint> indices, vector<Texture> textures);
  ~AssimpMesh();

	void Draw();
  virtual size_t GetVertexCount() const override { return vertices.size();}
  virtual std::vector<const Texture*> GetTextures() const override;

  /*  Mesh Data  */
  vector<AssimpVertex>	vertices;
  vector<GLuint>			  indices;
  vector<Texture>			  textures;

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();
  Texture default_diffuse_mapping;
  Texture default_specular_mapping;
  Texture default_normal_mapping;
};
