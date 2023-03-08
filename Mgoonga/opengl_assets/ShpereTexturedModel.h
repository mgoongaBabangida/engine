#pragma once
#include "opengl_assets.h"
#include <base/interfaces.h>

//---------------------------------------
class DLL_OPENGL_ASSETS SphereTexturedMesh : public IMesh
{
public:
  friend class SphereTexturedModel;
  SphereTexturedMesh();
  virtual ~SphereTexturedMesh();

  virtual void Draw();
  virtual size_t GetVertexCount() const { return vertices.size(); }
  virtual const std::string& Name() const override { return "SphereTexturedMesh"; }
  virtual std::vector<const Texture*> GetTextures() const { return {}; }
  virtual void SetMaterial(const Material&) override;
  virtual std::optional<Material> GetMaterial() const override;
  virtual bool HasMaterial() const override { return true; }

protected:
  Material material;

  unsigned int sphereVAO = 0;
  unsigned int indexCount;
  unsigned int vbo;
  unsigned int ebo;

  std::vector<MyVertex> vertices;
  std::vector<unsigned int> indices;
};

//------------------------------------------
class DLL_OPENGL_ASSETS SphereTexturedModel : public IModel
  {
  public:
    SphereTexturedModel(SphereTexturedMesh* _mesh, std::vector<const Texture*> t = std::vector<const Texture*>{})
      :m_mesh(_mesh), m_textures(t)
      {}

    virtual void						          Draw() override;
    virtual std::vector<glm::vec3>		GetPositions() const override;
    virtual std::vector<unsigned int>	GetIndeces() const override { return m_mesh->indices; }
    virtual size_t                    GetVertexCount() const override { return m_mesh->GetVertexCount();}
    virtual size_t                    GetMeshCount() const override { return 1; }
    virtual std::vector<const IMesh*> GetMeshes() const override { return { m_mesh.get() }; }
    virtual size_t                    GetAnimationCount() const override { return 0; }
    virtual std::vector<const IAnimation*> GetAnimations() const override { return {}; }
    virtual std::vector<const Texture*> GetTexturesModelLevel() const {
      return m_textures;
      }
  protected:
    std::vector<const Texture*> m_textures;
    std::shared_ptr<SphereTexturedMesh> m_mesh;
  };
