#pragma once

#include <assert.h>

#include <base/interfaces.h>
#include "Texture.h"

//--------------------------------------------------
class eScreenMesh : public IMesh
{
public:
	eScreenMesh(Texture textureOne, Texture textureTwo);
  ~eScreenMesh();

	virtual void Draw() override;
  virtual size_t GetVertexCount() const { return 4; }
  virtual std::vector<const Texture*> GetTextures() const;

	void SetTextureOne(Texture t) { textureOne = t; }
	void SetTextureTwo(Texture t) { textureTwo = t; }

protected:
	GLuint	quadVAO;
	GLuint	quadVBO;

	Texture textureOne;
	Texture textureTwo;
};

//---------------------------------------------------------
class eFrameMesh : public IMesh
{
public:
  eFrameMesh();
	~eFrameMesh();

	virtual void Draw() override;
  virtual size_t GetVertexCount() const { return 4; }
  virtual std::vector<const Texture*> GetTextures() const { assert("no textures in this private mesh"); return std::vector<const Texture*>(); }
	void UpdateFrame(float top_x, float top_y, float botom_x , float botom_y, float viewport_width, float viewport_height);

protected:
	GLuint	quadVAO_fr;
	GLuint	quadVBO_fr;
	std::array<GLfloat, 20> frameVertices;
};

