#pragma once

#include <opengl_assets\opengl_assets.h>

#include <map>
#include <string>

struct Texture;

class DLL_OPENGL_ASSETS eTextureManager
{
public:
	eTextureManager() = default;
	~eTextureManager();

	void		  InitContext(const std::string& folderPath);
	void		  LoadAllTextures();
	Texture*	Find(const std::string&);
	void		  AddTextureBox(const Texture&, const std::string&);

private:
	std::map<std::string, Texture>	m_Textures;
	std::string						folderPath;
};