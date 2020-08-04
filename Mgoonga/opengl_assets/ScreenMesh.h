#pragma once

#include <base/interfaces.h>
#include "Texture.h"

//--------------------------------------------------
class eScreenMesh : public IMesh
{
public:
	eScreenMesh(Texture textureOne, Texture textureTwo);
	virtual void Draw() override;
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
	virtual void Draw() override;

protected:
	GLuint	quadVAO_fr;
	GLuint	quadVBO_fr;
};
