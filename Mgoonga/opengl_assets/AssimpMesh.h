#pragma once

#include "Texture.h"
#include <base/interfaces.h>

//-----------------------------------------------------------------
class AssimpMesh : public IMesh
{
public:
	/*  Functions  */
	AssimpMesh(std::vector<AssimpVertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures ,const std::string& name = "Default");
  virtual ~AssimpMesh();

  AssimpMesh(const AssimpMesh&) = delete;
  AssimpMesh(AssimpMesh&&) = default;

  void FreeTextures();
	void Draw();
  virtual size_t GetVertexCount() const override { return vertices.size();}
  virtual std::vector<const Texture*> GetTextures() const override;
  virtual const std::string& Name() const override { return name;}
  /*  Mesh Data  */
  std::vector<AssimpVertex>	vertices;
  std::vector<GLuint>			  indices;
  std::vector<Texture>			textures;
  std::string               name;

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();

  //@todo make static
  Texture default_diffuse_mapping;
  Texture default_specular_mapping;
  Texture default_normal_mapping;
};
