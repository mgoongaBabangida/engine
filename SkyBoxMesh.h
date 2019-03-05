#pragma once

#include "InterfacesDB.h"
#include "Texture.h"

class eSkyBoxMesh : public IMesh
{
public:
	eSkyBoxMesh(Texture);
	void Draw() override;

protected:
	GLuint	skyboxVAO;
	GLuint	skyboxVBO;
	Texture texture;
};
