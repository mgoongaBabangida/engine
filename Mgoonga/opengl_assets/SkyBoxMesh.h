#pragma once

#include <base/interfaces.h>
#include "Texture.h"

//----------------------------------------
class eSkyBoxMesh : public IMesh
{
public:
	eSkyBoxMesh(Texture*);
	void Draw() override;
	void SetTexture(Texture* _t);

protected:
	GLuint	 skyboxVAO;
	GLuint	 skyboxVBO;
	Texture* texture;
};
