#include "stdafx.h"
#include "TextureImplDevIl.h"

uint8_t eTextureImplDevIl::LoadTexture(const std::string& path, uint32_t& id, int32_t& width, int32_t& height)
{
	//Generate and set current image ID
	ilGenImages(1, (ILuint*)&id);
	ilBindImage(id);

	//Load image
	ILboolean success = ilLoadImage((wchar_t*)path.c_str());
	std::cout << path.c_str() << std::endl;
	int i = ilGetError();
	if (!success)
	{
		std::cout << "error loading image" << std::endl;
		if (i == IL_COULD_NOT_OPEN_FILE)
		{
			std::cout << "IL_COULD_NOT_OPEN_FILE" << std::endl;
		}
		else if (i == IL_ILLEGAL_OPERATION)
		{
			std::cout << "IL_ILLEGAL_OPERATION" << std::endl;
		}
		else if (i == IL_INVALID_PARAM)
		{
			std::cout << "IL_INVALID_PARAM" << std::endl;
		}
		else if (i == IL_INVALID_EXTENSION)
		{
			std::cout << "IL_INVALID_EXTENSION" << std::endl;
		}
	}
	//Image loaded successfully
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	return 4; //RGBA
}

void eTextureImplDevIl::AssignPixels(uint8_t* buffer, int32_t width, int32_t height)
{
	//Convert image to RGBA
	ILboolean success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!success)
		std::cout << "error converting image" << std::endl;
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, buffer);
}

void eTextureImplDevIl::DeleteImage(uint32_t id)
{
	ilDeleteImages(1, (const ILuint*)&id);
}

void eTextureImplDevIl::SaveToFile(const uint8_t * buffer, const std::string & path, int32_t width, int32_t height, int32_t channels)
{
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilTexImage(
		width,
		height,
		1,  // OpenIL supports 3d textures!  but we don't want it to be 3d.  so
			// we just set this to be 1
		channels,  // 3 channels:  one for R , one for G, one for B
		IL_RGBA,  // duh, yeah use rgb!  coulda been rgba if we wanted trans
		IL_UNSIGNED_BYTE,  // the type of data the imData array contains (next)
		(void*)buffer  // and the array of bytes represneting the actual image data
	);

	ilEnable(IL_FILE_OVERWRITE);

	// actually save out as png
	ilSave(IL_PNG, (const wchar_t*)path.c_str());
	// now try saving as jpg
	//ilSave(IL_JPG, (const wchar_t*)"output.jpg");
	// now save as bmp
	//ilSave(IL_BMP, (const wchar_t*) "output.bmp");

	ilDeleteImages(1, &imageID);
}
