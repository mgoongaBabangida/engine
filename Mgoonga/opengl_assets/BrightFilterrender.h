#pragma once

#include "Texture.h"
#include "Shader.h"

class eBrightFilterRender
{
	Texture texture;
	Shader shader;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint textureLoc;
	GLuint TexWidthLoc;
public:
	eBrightFilterRender(GLuint				widtht, 
						GLuint				Height, 
						const std::string&	vS,
						const std::string&	fS);
	void Render();
	void SetTexture(Texture t) { texture = t; }
};
