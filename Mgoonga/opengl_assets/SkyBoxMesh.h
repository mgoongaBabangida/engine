#pragma once

#include <base/interfaces.h>
#include "Texture.h"

//----------------------------------------
class eSkyBoxMesh : public IMesh
{
public:
	eSkyBoxMesh(Texture*);
  ~eSkyBoxMesh();

	void Draw() override;
  virtual size_t GetVertexCount() const { return 4; }
	virtual const std::string& Name() const override { return name; }
  virtual std::vector<const Texture*> GetTextures() const;

	void SetTexture(Texture* _t);

protected:
	GLuint	 skyboxVAO;
	GLuint	 skyboxVBO;
	Texture* texture;
	std::string name;
};
