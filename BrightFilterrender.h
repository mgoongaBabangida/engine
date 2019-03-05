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
	eBrightFilterRender(GLuint widtht, GLuint Height);
	void Render();
	void SetTexture(Texture t) { texture = t; }
};
