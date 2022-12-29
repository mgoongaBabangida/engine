#pragma once

#include <base/interfaces.h>

#include "opengl_assets.h"

#include <vector>
#include <memory>

#include "MyMesh.h"

struct Texture;

class DLL_OPENGL_ASSETS MyModel: public IModel
{
public:
	//todo make one constructor
	MyModel();
  MyModel(std::shared_ptr<MyMesh> m, Texture* t);
  MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2);
  MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3);
	MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3, Texture* t4);
	MyModel(const MyModel& _other);

	virtual ~MyModel();

	std::vector<MyMesh*>			        getMeshes()		const;

	virtual std::vector<glm::vec3>	  GetPositions()	const	override;
	virtual std::vector<GLuint>		    GetIndeces()	const	override;
  virtual size_t                    GetVertexCount() const override;
  virtual size_t                    GetMeshCount() const override { return 1; }
  virtual std::vector<const IMesh*> GetMeshes() const override;
  virtual size_t                    GetAnimationCount() const { return 0; }
  virtual std::vector<const IAnimation*> GetAnimations() const { return std::vector<const IAnimation*>();}
  virtual std::vector<const Texture*> GetTexturesModelLevel() const override;

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
