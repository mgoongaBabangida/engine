#pragma once

#include "stdafx.h"
#include <opengl_assets\opengl_assets.h>

#include <map>

struct Texture;

class DLL_OPENGL_ASSETS eTextureManager
{
public:
	eTextureManager() = default;
	~eTextureManager();

	void			InitContext(const std::string& folderPath);
	void			LoadAllTextures(); //rename
	const Texture*	Find(const std::string&) const;
	const Texture*	FindByID(unsigned int _id) const;
	void			AddTextureBox(const Texture&, const std::string&);
	uint64_t	LoadTexture(const std::string& _path, const std::string& _name, const std::string& _type = {});

private:
	std::map<std::string, Texture>	m_Textures;
	std::string											folderPath;
};