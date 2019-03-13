#pragma once

#include "InterfacesDB.h"
#include "Shader.h"
#include "MyMesh.h"

struct Texture;

class MyModel: public IModel
{
protected:
	std::shared_ptr<MyMesh> mesh;
	Texture*				m_diffuse;
	Texture*				m_specular;
	Texture*				m_bump;
	Texture*				m_fourth;

public:
	MyModel() {}
	MyModel(std::shared_ptr<MyMesh> m, Texture* t) :mesh(m), m_diffuse(t), m_specular(t), m_bump(t), m_fourth(t) {  }
	MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2) :mesh(m),m_diffuse(t), m_specular(t2),m_bump(t), m_fourth(t) {  }
	MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3) :mesh(m), m_diffuse(t), m_specular(t2), m_bump(t3), m_fourth(t) {  }
	MyModel(std::shared_ptr<MyMesh> m, Texture* t, Texture* t2, Texture* t3, Texture* t4) :mesh(m), m_diffuse(t), m_specular(t2), m_bump(t3), m_fourth(t4) {  }
	
	vector<MyMesh*>					getMeshes()		const	{ vector<MyMesh*> meshes; meshes.push_back(mesh.get()); return meshes;}
	virtual std::vector<glm::vec3>	getPositions()	const	override;
	virtual std::vector<GLuint>		getIndeces()	const	override;

	virtual void					Draw()					override;
	void							Debug();
	
	void SetTexture(Texture* t)			{ m_diffuse = t; m_specular = t; m_bump = t; } // bump ? 
	void setTextureDiffuse(Texture* t)	{ m_diffuse = t; }
	void setTextureSpecular(Texture* t)	{ m_specular = t; }
	void setTextureBump(Texture* t)		{ m_bump = t; }
	void setTextureFourth(Texture* t)	{ m_fourth = t; }
};