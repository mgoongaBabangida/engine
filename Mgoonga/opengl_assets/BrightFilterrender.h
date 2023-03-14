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

	Shader& GetShader() { return shader; }
private:
	Shader shader;
  Texture texture;
  GLuint quadVAO;
  GLuint quadVBO;
};
