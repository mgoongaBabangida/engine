#include "stdafx.h"
#include "Texture.h"
#include <glm/glm/gtc/noise.hpp>
#include "TextureImplDevIl.h"
#include "TextureImplSDL.h"

#include <math/Random.h>

bool Texture::loadTextureFromFile(const std::string& _path, GLenum format, GLenum wrap)
{
	path		= _path;
	uint32_t ilId;
	mChannels = eTextureImplDevIl::LoadTexture(path, ilId, mTextureWidth, mTextureHeight);
	uint8_t* data = nullptr;
	eTextureImplDevIl::AssignPixels(data, mTextureWidth, mTextureHeight);

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
				(GLubyte*)data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (type == "terrain")
		glGenerateMipmap(id);

	eTextureImplDevIl::DeleteImage(ilId);
	return true;
}

uint8_t* Texture::getPixelBuffer()
{
	int sizeOfByte = sizeof(unsigned char);
	int bytesToUsePerPixel = mChannels;
	int theSize = mTextureWidth * mTextureHeight * sizeOfByte * bytesToUsePerPixel;
	uint8_t* imData = (uint8_t*)malloc(theSize);

	glBindTexture(GL_TEXTURE_2D, this->id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)imData); //generic
	glBindTexture(GL_TEXTURE_2D, 0);
	return imData;
}

bool Texture::saveToFile(const std::string &path)
{
	uint8_t* imData = getPixelBuffer();
	eTextureImplDevIl::SaveToFile(imData, path, mTextureWidth, mTextureHeight, mChannels);
	free(imData);
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
  //unsigned char* image;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);
  for (GLuint i = 0; i < faces.size(); i++)
  {
    //	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages(1, &imgID);
    ilBindImage(imgID);

    //Load image
    //ILboolean success = ilLoadImage( path.c_str() );
    ILboolean success = ilLoadImage((const wchar_t*)(faces[i].c_str)());
    int i0 = ilGetError();
    if (!success)
      std::cout << "error loading image" << std::endl;
    int i1 = IL_COULD_NOT_OPEN_FILE;
    int i2 = IL_ILLEGAL_OPERATION;
    int i3 = IL_INVALID_PARAM;
    //Image loaded successfully
    mTextureWidth = ilGetInteger(IL_IMAGE_WIDTH);
    mTextureHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    auto pixmap = new BYTE[mTextureWidth * mTextureHeight * 3];
    ilCopyPixels(0, 0, 0, mTextureWidth, mTextureHeight, 1, IL_RGB, IL_UNSIGNED_BYTE, pixmap);
    //Convert image to RGBA
    success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    if (!success)
      std::cout << "error converting image" << std::endl;
    // Load textures
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLuint*)ilGetData());
    //
    ilDeleteImages(1, &imgID);
    //
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  mChannels = 4;

  return true;
}

bool Texture::makeCubemap(Texture* _texture)
{
	mTextureWidth	= _texture->mTextureWidth;
	mTextureHeight  = _texture->mTextureHeight;
	mChannels		= 4;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	uint8_t* pixmap = (uint8_t*)malloc(mTextureWidth * mTextureHeight * sizeof(unsigned char) * mChannels);
	for (GLuint i = 0; i < 6; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, _texture->id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixmap);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)pixmap);
	}
	free(pixmap);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}

bool Texture::makeDepthTexture()
{
	assert(mTextureHeight);
	assert(mTextureWidth);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mTextureWidth, mTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	GLfloat borderColor[] = { 1.0,  1.0,  1.0,  1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	return true;
}

bool Texture::makeDepthCubeMap()
{
	assert(mTextureHeight);
	assert(mTextureWidth);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			mTextureWidth, mTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

bool Texture::makeRandom1DTexture(unsigned int _size)
{
	glm::vec3* pRandomData = new glm::vec3[_size];

	for (unsigned int i = 0; i < _size; i++) {
		pRandomData[i].x = math::Random::RandomFloat(0.0f, 1.0f);
		pRandomData[i].y = math::Random::RandomFloat(0.0f, 1.0f);
		pRandomData[i].z = math::Random::RandomFloat(0.0f, 1.0f);
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_1D, id);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, _size, 0.0f, GL_RGB, GL_FLOAT, pRandomData);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] pRandomData;

	return true;
}

bool Texture::generatePerlin(GLuint Width, GLuint Height, bool periodic)
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
	
	delete[] data;
	return true;
}

