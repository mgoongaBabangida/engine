#pragma once
#include "opengl_assets.h"
#include <base/interfaces.h>

//---------------------------------------
class DLL_OPENGL_ASSETS SphereTexturedMesh : public I3DMesh //@todo it has to be just MyMesh !!!
{
public:
  friend class SphereTexturedModel;
  
  SphereTexturedMesh();
  virtual ~SphereTexturedMesh();

  virtual const std::string& Name() const override { return name; }
  virtual void Draw();

  virtual size_t                            GetVertexCount() const { return vertices.size(); }
  virtual const std::vector<Vertex>&        GetVertexs() const override { return vertices; }
  virtual const std::vector<unsigned int>&  GetIndices() const override { return indices; }
  virtual void                              BindVAO() const override;
  virtual void                              UnbindVAO() const override;

  virtual std::vector<TextureInfo>        GetTextures() const override { return {}; }
  virtual void                            AddTexture(Texture*) {}

  virtual void SetMaterial(const Material&) override;
  virtual std::optional<Material> GetMaterial() const override;
  virtual bool HasMaterial() const override { return true; }

protected:
  std::string name = "SphereTexturedMesh";
  Material m_material;

  unsigned int sphereVAO = 0;
  unsigned int indexCount;
  unsigned int vbo;
  unsigned int ebo;

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
};

//------------------------------------------
class DLL_OPENGL_ASSETS SphereTexturedModel : public IModel
  {
  public:
    explicit SphereTexturedModel(SphereTexturedMesh* _mesh)
      :m_mesh(_mesh)
      {}

    virtual void						          Draw() override;
    virtual const std::string&        GetName() const override { return m_mesh->Name(); }
    virtual const std::string&        GetPath() const override { return m_path; }

    virtual size_t                      GetVertexCount() const override { return m_mesh->GetVertexCount();}
    virtual size_t                      GetMeshCount() const override { return 1; }
    virtual std::vector<const IMesh*>   GetMeshes() const override { return { m_mesh.get() }; }
    virtual std::vector<const I3DMesh*> Get3DMeshes() const override { return { m_mesh.get() }; }

    virtual bool														HasBones() const { return false; }
    virtual std::vector<const IBone*>				GetBones() const { return {}; }

    virtual size_t                          GetAnimationCount() const override { return 0; }
    virtual std::vector<const IAnimation*>  GetAnimations() const override { return {}; }

  protected:
    std::shared_ptr<SphereTexturedMesh> m_mesh;
    std::string                         m_path;
  };
