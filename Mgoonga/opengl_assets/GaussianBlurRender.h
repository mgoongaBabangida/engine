#pragma once

#include "Texture.h"
#include "Shader.h"

//---------------------------------------------
class eGaussianBlurRender
{
public:
	eGaussianBlurRender(GLuint	width,
						GLuint				height, 
						const std::string&	vS,
						const std::string&	fS);
  virtual ~eGaussianBlurRender();

	void Render();
	void SetTexture(Texture t) { texture = t; }

private:
  Texture texture;
  Shader shader;

  GLuint quadVAO;
  GLuint quadVBO;

  GLuint textureLoc;
  GLuint TexWidthLoc;

  GLuint width;
  GLuint height;
};