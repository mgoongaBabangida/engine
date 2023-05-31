#pragma once

#include <base/interfaces.h>
#include "Texture.h"

//----------------------------------------
class eSkyBoxMesh : public IMesh
{
public:
	eSkyBoxMesh(const Texture*);
  ~eSkyBoxMesh();

	void Draw() override;
  virtual size_t GetVertexCount() const { return 4; }
	virtual const std::string& Name() const override { return name; }
  virtual std::vector<const Texture*> GetTextures() const;

	void SetTexture(const Texture* _t);

protected:
	GLuint	 skyboxVAO;
	GLuint	 skyboxVBO;
	const Texture* texture;
	std::string name;
};
