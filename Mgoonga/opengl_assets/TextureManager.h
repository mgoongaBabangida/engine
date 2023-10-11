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

	void						Initialize();
	void						InitContext(const std::string& folderPath);

	const Texture*	Find(const std::string&) const;
	const Texture*	FindByID(unsigned int _id) const;

	const std::vector<unsigned int>& GetCubeMapIds() const { return m_cubemap_ids; }
	const std::vector<unsigned int>& GetHdrIds() const { return m_hdr_ids; }
	const std::vector<std::pair<unsigned int, unsigned int>>& GetIBLIds() const { return m_ibl_ids; }

	void AddCubeMapId(unsigned int _id) { m_cubemap_ids.push_back(_id); }
	void AddIBLId(unsigned int _irr, unsigned int _prefilter) { m_ibl_ids.push_back({ _irr, _prefilter }); }

	void						AddExisting(const std::string&, Texture*);

	uint64_t				LoadTexture(const std::string& _path, const std::string& _name, const std::string& _type = {});

protected:
	void						_LoadHardcoded();

	std::map<std::string, Texture>	 m_Textures;
	std::string											 folderPath;
	std::vector<unsigned int>				 m_cubemap_ids;
	std::vector<unsigned int>				 m_hdr_ids;
	std::vector<std::pair<unsigned int, unsigned int>> m_ibl_ids;
};