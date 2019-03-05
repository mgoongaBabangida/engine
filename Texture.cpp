#include "Texture.h"
#include <IL/IL.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <glm/gtc/noise.hpp>

bool Texture::loadTextureFromFile(const std::string & path, GLenum format, GLenum wrap)
{
	//freeTexture();
	//Generate and set current image ID
	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);

	//Load image
	//ILboolean success = ilLoadImage( path.c_str() );
	ILboolean success = ilLoadImage((const char*)(path.c_str)());
	std::cout << path.c_str() << std::endl;
	int i = ilGetError();
	if (!success)
	 std::cout << "error loading image" << std::endl;
	int i1 = IL_COULD_NOT_OPEN_FILE;
	int i2 = IL_ILLEGAL_OPERATION;
	int i3 = IL_INVALID_PARAM;
	//Image loaded successfully
	mTextureWidth = ilGetInteger(IL_IMAGE_WIDTH);
	mTextureHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	//std::cout << ilGetInteger(IL_IMAGE_BPP) << std::endl;

	auto pixmap = new BYTE[mTextureWidth * mTextureHeight * 4]; /////
	//Convert image to RGBA
	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!success)
		std::cout << "error converting image" << std::endl;
	ilCopyPixels(0, 0, 0, mTextureWidth, mTextureHeight, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap); //////
	// Load textures
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLuint*)ilGetData());
	mChannels = 4;//RGBA
	//
	ilDeleteImages(1, &imgID);
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	delete[] pixmap;
	return success;
	
}

bool Texture::saveToFile( const std::string &path )
{
	int sizeOfByte = sizeof(unsigned char); //!??
	int bytesToUsePerPixel = 4;  // RGB
	int theSize = mTextureWidth * mTextureHeight * sizeOfByte * bytesToUsePerPixel;
	unsigned char * imData = (unsigned char*)malloc(theSize);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_UNSIGNED_BYTE,(void*)imData); //generic
	glBindTexture(GL_TEXTURE_2D, 0);
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	
	ilTexImage(	
		mTextureWidth,
		mTextureHeight,
		1,  // OpenIL supports 3d textures!  but we don't want it to be 3d.  so
			// we just set this to be 1
		4,  // 3 channels:  one for R , one for G, one for B
		IL_RGBA,  // duh, yeah use rgb!  coulda been rgba if we wanted trans
		IL_UNSIGNED_BYTE,  // the type of data the imData array contains (next)
		imData  // and the array of bytes represneting the actual image data
	);

	ilEnable(IL_FILE_OVERWRITE);

	// actually save out as png
	ilSave(IL_PNG, (const char*) path.c_str());

	// now try saving as jpg
	//ilSave(IL_JPG, (const wchar_t*)"output.jpg");

	// now save as bmp
	//ilSave(IL_BMP, (const wchar_t*) "output.bmp");

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

void Texture::Render(GLfloat x, GLfloat y)
{

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
		ILboolean success = ilLoadImage((const char*)(faces[i].c_str)());
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
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLuint*)ilGetData());
		//
		ilDeleteImages(1, &imgID);
		//
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	mChannels = 4;
	return true;
}

bool Texture::generatePerlin(GLuint Width, GLuint Height,bool periodic)
{
	this->mTextureWidth = Width;
	this->mTextureHeight = Height;
	int a = 1; int b = 2;
	GLubyte *data = new GLubyte[Width*Height * 4];
	float xFactor = 1.0f / (Width - 1);
	float yFactor = 1.0f / (Height - 1);
	mChannels = 4; //octaves
	for (int row = 0; row < Height; row++) {
		for (int col = 0; col < Width; col++) {  //row?
			float x = xFactor* col;
			float y = yFactor * row;
			float sum = 0;
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

