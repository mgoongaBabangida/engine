#pragma once

#include "Texture.h"
#include "Shader.h"

//-----------------------------------------
class eBrightFilterRender
{
public:
	eBrightFilterRender(GLuint				widtht,
						GLuint				Height, 
						const std::string&	vS,
						const std::string&	fS);
  ~eBrightFilterRender();
	void Render();
	void SetTexture(Texture t) { texture = t; }

private:
  Texture texture;
  Shader shader;
  GLuint quadVAO;
  GLuint quadVBO;
};
