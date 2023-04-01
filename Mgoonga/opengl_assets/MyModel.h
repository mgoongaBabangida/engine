#pragma once

#include <base/interfaces.h>

#include "opengl_assets.h"

#include "MyMesh.h"

struct Texture;

//---------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyModel: public IModel
{
public:
	//todo make one constructor
	MyModel();
	MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2 = nullptr, Texture* t3 = nullptr, Texture* t4 = nullptr);
	MyModel(const MyModel& _other);

	virtual ~MyModel();

	std::vector<MyMesh*>											getMeshes()		const;

	virtual std::vector<glm::vec3>						GetPositions()	const	override;
	virtual std::vector<GLuint>								GetIndeces()	const	override;
  virtual size_t														GetVertexCount() const override;
  virtual size_t														GetMeshCount() const override { return 1; }
  virtual std::vector<const IMesh*>					GetMeshes() const override;
  virtual size_t														GetAnimationCount() const { return 0; }
  virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>();}
  virtual std::vector<const Texture*>				GetTexturesModelLevel() const override;

	virtual void					Draw()					override;
	void							    Debug();
	
	void SetTexture(Texture* t);
	void setTextureDiffuse(Texture* t);
	void setTextureSpecular(Texture* t);
	void setTextureBump(Texture* t);
	void setTextureFourth(Texture* t);

protected:
	std::shared_ptr<MyMesh> mesh;
	Texture*				m_diffuse;
	Texture*				m_specular;
	Texture*				m_bump;
	Texture*				m_fourth;
};

//-------------------------------------------------------
class DLL_OPENGL_ASSETS SimpleModel : public IModel
{
public:
	SimpleModel(IMesh* _m) :m_mesh(_m) {}

	virtual ~SimpleModel() { delete m_mesh; }

	virtual std::vector<glm::vec3>						GetPositions()	const	override { return std::vector<glm::vec3>{}; }
	virtual std::vector<GLuint>								GetIndeces()	const	override { return std::vector<GLuint>{}; }
	virtual size_t														GetVertexCount() const override { return 0; }
	virtual size_t														GetMeshCount() const override { return 1; }
	virtual std::vector<const IMesh*>					GetMeshes() const override { return std::vector<const IMesh*>{ m_mesh }; }
	virtual size_t														GetAnimationCount() const { return 0; }
	virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>(); }
	virtual std::vector<const Texture*>				GetTexturesModelLevel() const override { return std::vector<const Texture*>{}; }

	virtual void					Draw()					override { m_mesh->Draw(); }

protected:
	IMesh* m_mesh;//-> unique_ptr
};


//-------------------------------------------------------
class DLL_OPENGL_ASSETS BezierCurveModel : public IModel
{
public:
	BezierCurveModel(BezierCurveMesh* _m) :m_mesh(_m) {}

	virtual ~BezierCurveModel() { delete m_mesh; }

	virtual std::vector<glm::vec3>						GetPositions()	const	override { return std::vector<glm::vec3>{}; }
	virtual std::vector<GLuint>								GetIndeces()	const	override { return std::vector<GLuint>{}; }
	virtual size_t														GetVertexCount() const override { return 0; }
	virtual size_t														GetMeshCount() const override { return 1; }
	virtual std::vector<const IMesh*>					GetMeshes() const override { return std::vector<const IMesh*>{ m_mesh }; }
	virtual size_t														GetAnimationCount() const { return 0; }
	virtual std::vector<const IAnimation*>		GetAnimations() const { return std::vector<const IAnimation*>(); }
	virtual std::vector<const Texture*>				GetTexturesModelLevel() const override { return std::vector<const Texture*>{}; }

	virtual void					Draw()					override { m_mesh->Draw(); }

protected:
	BezierCurveMesh* m_mesh; //-> unique_ptr
};