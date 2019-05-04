#pragma once
#include <map>
#include "Texture.h"

class eTextureManager
{
public:
	eTextureManager() = default;
	~eTextureManager();
	void		InitContext(const std::string& folderPath);
	void		LoadAllTextures();
	Texture*	Find(const std::string&);
private:
	std::map<std::string, Texture>	m_Textures;
	std::string						folderPath;
};