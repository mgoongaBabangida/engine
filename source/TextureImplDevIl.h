#pragma once
#include <stdint.h>
#include <string>

class eTextureImplDevIl
{
public:
	static uint8_t  LoadTexture(const std::string& path, uint32_t& id, int32_t& width, int32_t& height);
	static void		AssignPixels(uint8_t* buffer, int32_t width, int32_t height);
	static void		DeleteImage(uint32_t id);
	static void		SaveToFile(const uint8_t* buffer, const std::string& path, int32_t width, int32_t height, int32_t channels);
};
