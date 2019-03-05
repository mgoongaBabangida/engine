#pragma once
#include "Texture.h"
#include "Shader.h"

class eGaussianBlurRender
{
	Texture texture;
	Shader shader;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint textureLoc;
	GLuint TexWidthLoc;
	GLuint width;
	GLuint height;

public:
	eGaussianBlurRender(GLuint width, GLuint height);
	void Render();
	void SetTexture(Texture t) { texture = t; }
	virtual ~eGaussianBlurRender() = default;
};