#pragma once

#include "Texture.h"
#include <base/interfaces.h>

//-----------------------------------------------------------------
class AssimpMesh : public IMesh
{
public:
  friend class Model;

	AssimpMesh(std::vector<AssimpVertex> vertices,
             std::vector<GLuint> indices,
             std::vector<Texture> textures,
             const Material material,
             const std::string& name = "Default");
  virtual ~AssimpMesh();

  AssimpMesh(const AssimpMesh&) = delete;
  AssimpMesh(AssimpMesh&&) = default;

  void FreeTextures();
	void Draw();
  virtual size_t GetVertexCount() const override { return vertices.size();}
  virtual const std::string& Name() const override { return name;}

  virtual bool											HasMaterial() const override { return true; }
  virtual void											SetMaterial(const Material&) override;
  virtual std::optional<Material>		GetMaterial() const override { return material; }

protected:
  /*  Mesh Data  */
  std::vector<AssimpVertex>	vertices;
  std::vector<GLuint>			  indices;
  std::vector<Texture>			textures;
  std::string               name;
  Material                  material;

	/*  Render data  */
	GLuint VAO, VBO, EBO;

	/*  Functions    */
	void setupMesh();

  static Texture default_diffuse_mapping;
  static Texture default_specular_mapping;
  static Texture default_normal_mapping;
  static Texture default_emission_mapping;
};
