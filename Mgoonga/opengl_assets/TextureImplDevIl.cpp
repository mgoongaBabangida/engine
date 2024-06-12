#include "stdafx.h"
#include "TextureImplDevIl.h"

#include <base/Log.h>

#ifdef DEVIL_IMAGE

static uint8_t* pixmap;

uint8_t eTextureImplDevIl::LoadTexture(const std::string& path, uint32_t& id, int32_t& width, int32_t& height)
{
	//Generate and set current image ID
	ilGenImages(1, (ILuint*)&id);
	ilBindImage(id);

	//Load image
	ILboolean success = ilLoadImage((wchar_t*)path.c_str());
	base::Log(path);
	int i = ilGetError();
	if (!success)
	{
		base::Log("error loading image");
		if (i == IL_COULD_NOT_OPEN_FILE)
		{
			base::Log("IL_COULD_NOT_OPEN_FILE");
		}
		else if (i == IL_ILLEGAL_OPERATION)
		{
			base::Log("IL_ILLEGAL_OPERATION");
		}
		else if (i == IL_INVALID_PARAM)
		{
			base::Log("IL_INVALID_PARAM");
		}
		else if (i == IL_INVALID_EXTENSION)
		{
			base::Log("IL_INVALID_EXTENSION");
		}
	}
	//Image loaded successfully
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	return 4; //RGBA
}

void eTextureImplDevIl::AssignPixels(uint8_t*& buffer, int32_t width, int32_t height)
{
	pixmap = new uint8_t[width * height * 4];
	buffer = pixmap;
	//Convert image to RGBA
	ILboolean success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!success)
		base::Log("error converting image");
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, buffer);
}

void eTextureImplDevIl::DeleteImage(uint32_t id)
{
	ilDeleteImages(1, (const ILuint*)&id);
	delete[] pixmap;
}

void eTextureImplDevIl::SaveToFile(const uint8_t * buffer, const std::string & _path, int32_t _width, int32_t _height, int32_t _layers, int32_t _channels, GLenum _data_type)
{
	auto il_format = IL_RGBA;
	if (_channels == 1)
		il_format = IL_ALPHA;
	else if (_channels == 3)
		il_format = IL_RGB;

	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilTexImage(
		_width,
		_height,
		1, // OpenIL supports 3d textures!  but we don't want it to be 3d.
		_channels, 
		il_format,
		_data_type,  // the type of data the imData array contains (next)
		(void*)buffer  // and the array of bytes represneting the actual image data
	);

	ilEnable(IL_FILE_OVERWRITE);

	if(_path.find(".png") != std::string::npos)
		ilSave(IL_PNG, (const wchar_t*)_path.c_str());
	else if (_path.find(".jpg") != std::string::npos)
		ilSave(IL_JPG, (const wchar_t*)_path.c_str());
	// now save as bmp
	//ilSave(IL_BMP, (const wchar_t*) "output.bmp");

	ilDeleteImages(1, &imageID);
}

#endif
