#pragma once

#include <base/interfaces.h>

#include "Texture.h"

DLL_OPENGL_ASSETS I3DMesh* MakeMesh(std::vector<Vertex> vertices,
                                    std::vector<GLuint> indices,
                                    std::vector<TextureInfo> textures,
                                    const Material& material,
                                    const std::string& name = "Default",
                                    bool _calculate_tangent = false);

//-----------------------------------------------------------------
class /*DLL_OPENGL_ASSETS*/ eMesh : public I3DMesh
{
public:
  friend class eModel;

	eMesh(std::vector<Vertex> vertices,
             std::vector<GLuint> indices,
             std::vector<Texture> textures,
             const Material& material,
             const std::string& name = "Default",
             bool _calculate_tangent = false);
  virtual ~eMesh();

  eMesh(const eMesh&) = delete;
  eMesh& operator=(const eMesh&) = delete;

  eMesh(eMesh&&) noexcept = default;
  eMesh& operator=(eMesh&&) noexcept = default;

  void Draw() override;
  void DrawInstanced(int32_t instances) override;

  void SetupMesh();
  void ReloadTextures();
  void FreeTextures();
  virtual void ReloadVertexBuffer() override;

  virtual const std::string& Name() const override { return name;}

  virtual size_t                            GetVertexCount() const override { return vertices.size(); }
  virtual const std::vector<Vertex>&        GetVertexs() const override { return vertices; }
  virtual const std::vector<unsigned int>&  GetIndices() const override { return indices; }
  virtual std::vector<TextureInfo>					GetTextures() const override;
  virtual void                              AddTexture(Texture*) override;

  virtual void                              BindVAO() const override { glBindVertexArray(this->VAO); }
  virtual void                              UnbindVAO() const override { glBindVertexArray(0); }

  virtual bool											HasMaterial() const override { return true; }
  virtual void											SetMaterial(const Material&) override;
  virtual std::optional<Material>		GetMaterial() const override { return m_material; }

  virtual void											calculatedTangent() override;

protected:
  void _BindRawTextures();
  void _BindMaterialTextures();

  /*  Mesh Data  */
  std::vector<Vertex>	    vertices;
  std::vector<GLuint>			indices;
  std::vector<Texture>		textures;
  std::string             name;
  Material                m_material;

	/*  Render data  */
	GLuint VAO, VBO, EBO;
};
