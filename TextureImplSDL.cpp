#include "TextureImplSDL.h"
#include <iostream>

static SDL_Surface* current_surface;

uint8_t eTextureImplSDL::LoadTexture(const std::string& path, uint32_t& id, int32_t& width, int32_t& height)
{
	current_surface = nullptr;
	current_surface = IMG_Load(path.c_str());
	if(current_surface == nullptr)
	{
		std::cout << IMG_GetError() << " Path is: " << path << std::endl;
	}
	width  = current_surface->w;
	height = current_surface->h;

	int mode;
	if (current_surface->format->BytesPerPixel == 4)
	{
		if (current_surface->format->Rmask == 0x000000ff)
		{
			mode = 0;// GL_RGBA;
		}
		else
		{
			mode = 1;//GL_BGRA;
		}
	}
	else if (current_surface->format->BytesPerPixel == 3)
	{
		if (current_surface->format->Rmask == 0x000000ff)
		{
			mode = 2;// GL_RGB;
		}
		else
		{
			mode = 3;// GL_BGR;
		}
	}
	return current_surface->format->BytesPerPixel;
}

void eTextureImplSDL::AssignPixels(uint8_t*& buffer, int32_t width, int32_t height)
{
	buffer = (uint8_t*)current_surface->pixels;
}

void eTextureImplSDL::DeleteImage(uint32_t id)
{
	SDL_FreeSurface(current_surface);
}

void eTextureImplSDL::SaveToFile(const uint8_t* buffer, const std::string& path, int32_t width, int32_t height, int32_t channels)
{

}
