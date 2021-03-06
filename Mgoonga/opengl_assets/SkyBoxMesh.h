#pragma once

#include <base/interfaces.h>
#include "Texture.h"

//----------------------------------------
class eSkyBoxMesh : public IMesh
{
public:
	eSkyBoxMesh(Texture*);
	void Draw() override;
  virtual size_t GetVertexCount() const { return 4; }
  virtual std::vector<const Texture*> GetTextures() const;

	void SetTexture(Texture* _t);

protected:
	GLuint	 skyboxVAO;
	GLuint	 skyboxVBO;
	Texture* texture;
};
