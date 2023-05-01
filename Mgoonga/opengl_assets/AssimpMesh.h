#pragma once

#include "Texture.h"
#include <base/interfaces.h>

//-----------------------------------------------------------------
class AssimpMesh : public IMesh
{
public:
  friend class Model;

	AssimpMesh(std::vector<Vertex> vertices,
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

  virtual const std::vector<Vertex>& GetVertexs() const override { return vertices; }
  virtual const std::vector<unsigned int>& GetIndices() const override { return indices; }

  virtual bool											HasMaterial() const override { return true; }
  virtual void											SetMaterial(const Material&) override;
  virtual std::optional<Material>		GetMaterial() const override { return m_material; }

protected:
  void _BindRawTextures();
  void _BindMaterialTextures();

  /*  Mesh Data  */
  std::vector<Vertex>	vertices;
  std::vector<GLuint>			  indices;
  std::vector<Texture>			textures;
  std::string               name;
  Material                  m_material;

	/*  Render data  */
	GLuint VAO, VBO, EBO;

	/*  Functions    */
	void setupMesh();

  static Texture default_diffuse_mapping;
  static Texture default_specular_mapping;
  static Texture default_normal_mapping;
  static Texture default_roughness_mapping; //anti-glossiness
  static Texture default_emission_mapping;
};
