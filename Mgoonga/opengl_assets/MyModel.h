#pragma once

#include <base/interfaces.h>

#include "opengl_assets.h"

#include "MyMesh.h"

struct Texture;

//---------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyModel: public IModel
{
public:
	//@todo make one constructor
	MyModel();
	MyModel(std::shared_ptr<MyMesh> _mesh,
					const std::string& _name,
					Texture* t = nullptr,
					Texture* t2 = nullptr,
					Texture* t3 = nullptr,
					Texture* t4 = nullptr);
	MyModel(const MyModel& _other);

	virtual ~MyModel();

	std::vector<MyMesh*>											getMeshes()		const;

  virtual size_t														GetVertexCount() const override;
  virtual size_t														GetMeshCount() const override { return 1; }
  virtual std::vector<const I3DMesh*>				Get3DMeshes() const override;
	virtual std::vector<const IMesh*>					GetMeshes() const override;

	virtual size_t														GetAnimationCount() const { return 0; }
  virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>();}

	virtual bool														HasBones() const { return false; }
	virtual std::vector<const IBone*>				GetBones() const { return {}; }

	virtual bool											HasMaterial() const { return true; }
	virtual void											SetMaterial(const Material& _material) { m_material = _material; }
	virtual std::optional<Material>		GetMaterial() const { return m_material; }

	virtual void								Draw()					override;
	virtual const std::string&	GetName() const override { return m_name; }
	virtual const std::string&	GetPath() const override { return m_path; }

	void												Debug();
	
	void SetTexture(Texture* t);
	void setTextureDiffuse(Texture* t);
	void setTextureSpecular(Texture* t);
	void setTextureBump(Texture* t);
	void setTextureFourth(Texture* t);

protected:
	std::shared_ptr<MyMesh> mesh;
	Material								m_material;
	std::string							m_name;
	std::string							m_path;

	static Texture default_diffuse_mapping;
	static Texture default_specular_mapping;
	static Texture default_normal_mapping;
	static Texture default_roughness_mapping;
	static Texture default_emission_mapping;
};

//-------------------------------------------------------
class DLL_OPENGL_ASSETS SimpleModel : public IModel //@todo need to improve
{
public:
	SimpleModel(IMesh* _m) :m_mesh(_m) {}

	virtual ~SimpleModel() { delete m_mesh; }

	virtual size_t														GetVertexCount() const override { return 0; }
	virtual size_t														GetMeshCount() const override { return 1; }
	virtual std::vector<const IMesh*>					GetMeshes() const override { return std::vector<const IMesh*>{ m_mesh }; }
	virtual std::vector<const I3DMesh*>				Get3DMeshes() const override { return std::vector<const I3DMesh*>{}; }
	virtual size_t														GetAnimationCount() const { return 0; }
	virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>(); }

	virtual bool														HasBones() const { return false; }
	virtual std::vector<const IBone*>				GetBones() const { return {}; }

	virtual void								Draw()					override { m_mesh->Draw(); }
	virtual const std::string&	GetName() const override { return m_mesh->Name(); }
	virtual const std::string&	GetPath() const override { return m_path; }

protected:
	IMesh*									m_mesh;//-> unique_ptr
	std::string							m_path;
};

//-------------------------------------------------------
class DLL_OPENGL_ASSETS BezierCurveModel : public IModel //@todo need to improve
{
public:
	BezierCurveModel(BezierCurveMesh* _m) :m_mesh(_m) {}

	virtual ~BezierCurveModel() { delete m_mesh; }

	virtual size_t														GetVertexCount() const override { return 0; }
	virtual size_t														GetMeshCount() const override { return 1; }
	virtual std::vector<const IMesh*>					GetMeshes() const override { return std::vector<const IMesh*>{ m_mesh }; }
	virtual std::vector<const I3DMesh*>				Get3DMeshes() const override { return std::vector<const I3DMesh*>{}; }
	virtual size_t														GetAnimationCount() const { return 0; }
	virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>(); }

	virtual bool														HasBones() const { return false; }
	virtual std::vector<const IBone*>				GetBones() const { return {}; }

	virtual void								Draw()					override { m_mesh->Draw(); }
	virtual const std::string&	GetName() const override { return m_mesh->Name(); }
	virtual const std::string&	GetPath() const override { return m_path; }
protected:
	BezierCurveMesh*				m_mesh; //-> unique_ptr
	std::string							m_path;
};