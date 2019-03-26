#pragma once
#include "Texture.h"

class TextureManager
{
private:
	std::map<std::string, Texture> m_Textures;
	std::string folderPath;
public:
	~TextureManager();
	void InitContext(const std::string& folderPath);
	void loadAllTextures();
	Texture* find(const std::string&);
};