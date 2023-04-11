#pragma once

#include "Texture.h"
#include <base/interfaces.h>

//-----------------------------------------------------------------
class AssimpMesh : public IMesh
{
public:
  friend class Model;

	AssimpMesh(std::vector<AssimpVertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures ,const std::string& name = "Default");
  virtual ~AssimpMesh();

  AssimpMesh(const AssimpMesh&) = delete;
  AssimpMesh(AssimpMesh&&) = default;

  void FreeTextures();
	void Draw();
  virtual size_t GetVertexCount() const override { return vertices.size();}
  virtual std::vector<const Texture*> GetTextures() const override;
  virtual const std::string& Name() const override { return name;}

protected:
  /*  Mesh Data  */
  std::vector<AssimpVertex>	vertices;
  std::vector<GLuint>			  indices;
  std::vector<Texture>			textures;
  std::string               name;

	/*  Render data  */
	GLuint VAO, VBO, EBO;

	/*  Functions    */
	void setupMesh();

  static Texture default_diffuse_mapping;
  static Texture default_specular_mapping;
  static Texture default_normal_mapping;
  static Texture default_emission_mapping;
};
