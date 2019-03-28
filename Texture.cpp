#include "Texture.h"
#include <glm\glm/gtc/noise.hpp>
#include "TextureImplDevIl.h"
#include "TextureImplSDL.h"

#include <iostream>

bool Texture::loadTextureFromFile(const std::string& _path, GLenum format, GLenum wrap)
{
	path		= _path;
	uint32_t ilId;
	mChannels = eTextureImplDevIl::LoadTexture(path, ilId, mTextureWidth, mTextureHeight);
	uint8_t* pixmap = nullptr;
	eTextureImplDevIl::AssignPixels(pixmap, mTextureWidth, mTextureHeight);

	// Load textures
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 
				0, 
				mChannels == 4 ? GL_RGBA : GL_RGB, 
				mTextureWidth, 
				mTextureHeight, 
				0, 
				mChannels == 4 ? GL_RGBA : GL_RGB, 
				GL_UNSIGNED_BYTE, 
				(GLubyte*)pixmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	eTextureImplDevIl::DeleteImage(ilId);
	return true;
}

bool Texture::saveToFile(const std::string &path)
{
	int sizeOfByte = sizeof(unsigned char);
	int bytesToUsePerPixel = mChannels;
	int theSize = mTextureWidth * mTextureHeight * sizeOfByte * bytesToUsePerPixel;
	uint8_t* imData = (uint8_t*)malloc(theSize);
	
	glBindTexture(GL_TEXTURE_2D, this->id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_UNSIGNED_BYTE,(void*)imData); //generic
	glBindTexture(GL_TEXTURE_2D, 0);
	
	eTextureImplDevIl::SaveToFile(imData, path, mTextureWidth, mTextureHeight, mChannels);
	return true;
}

void Texture::freeTexture()
{
	//Delete texture
	if (id != 0)
	{
		glDeleteTextures(1, &id);
		id = 0;
	}
	mTextureWidth = 0;
	mTextureHeight = 0;
}

bool Texture::loadTexture1x1(TColor color)
{
	GLubyte tex[4];
	switch (color) {
	case WHITE:
		tex[0]=255, tex[1] = 255, tex[2] = 255, tex[3] = 255;
		break;
	case BLACK:
		tex[0] = 0, tex[1] = 0, tex[2] = 0, tex[3] = 255;
		break;
	case BLUE:
		tex[0] = 128, tex[1] = 128, tex[2] = 255, tex[3] = 255;
		break;
	case PINK:
		tex[0] = 232, tex[1] = 100, tex[2] = 214, tex[3] = 255;
		break;
	case YELLOW:
		tex[0] = 255, tex[1] = 255, tex[2] = 0, tex[3] = 255;
		break;
	}
	// Load textures
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	mTextureWidth = 1;
	mTextureHeight = 1;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
	mChannels = 4;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}

bool Texture::loadCubemap(std::vector<std::string> faces)
{
	mChannels = 4;
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		uint32_t ilId;
		eTextureImplDevIl::LoadTexture(faces[i], ilId, mTextureWidth, mTextureHeight);
		uint8_t* pixmap;
		eTextureImplDevIl::AssignPixels(pixmap, mTextureWidth, mTextureHeight);
		// Load textures
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)pixmap);

		eTextureImplDevIl::DeleteImage(ilId);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}

bool Texture::generatePerlin(GLuint Width, GLuint Height,bool periodic)
{
	this->mTextureWidth = Width;
	this->mTextureHeight = Height;
	float a = 1.0f; float b = 2.0f;
	GLubyte *data = new GLubyte[Width*Height * 4];
	float xFactor = 1.0f / (Width - 1);
	float yFactor = 1.0f / (Height - 1);
	mChannels = 4; //octaves
	for (uint32_t row = 0; row < Height; row++) {
		for (uint32_t col = 0; col < Width; col++) {  //row?
			float x = xFactor* col;
			float y = yFactor * row;
			float sum = 0.0f;
			float freq = a;
			float scale = b;
			for (int oct = 0; oct < 4; ++oct) {
				glm::vec2 p(x * freq, y * freq);
				float val = 0.0f;
					if(periodic) {
						val = glm::perlin(p, glm::vec2(freq) ) / scale;
					}
					else {
					val = glm::perlin(p) / scale;
					}
				sum += val;
				float result = (sum + 1.0f) / 2.0f;

				data[((row*Width + col) * 4) + oct] = (GLubyte)(result*255.0f);
				freq *= 2.0f;
				scale *= b;
			}
		}
	}
	
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->mTextureWidth, this->mTextureHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->mTextureWidth, this->mTextureHeight, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);

	return true;
}

