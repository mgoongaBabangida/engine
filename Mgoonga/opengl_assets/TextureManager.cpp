#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"

#include <fstream>
#include <sstream>

#define DEVIL_IMAGE
#define SDL_IMAGE

#ifdef DEVIL_IMAGE
	#include "TextureImplDevIl.h"
#endif

#ifdef SDL_IMAGE
	#include "TextureImplSDL.h"
#endif

//-----------------------------------------------------------------------------
eTextureManager::~eTextureManager()
{
  for (auto& node : m_Textures)
    node.second.freeTexture();

	// free static textures
	Texture::freeTexture(Texture::GetTexture1x1(WHITE).id);
	Texture::freeTexture(Texture::GetTexture1x1(BLACK).id);
	Texture::freeTexture(Texture::GetTexture1x1(BLUE).id);
	Texture::freeTexture(Texture::GetTexture1x1(PINK).id);
	Texture::freeTexture(Texture::GetTexture1x1(YELLOW).id);
	Texture::freeTexture(Texture::GetTexture1x1(GREY).id);

#ifdef SDL_IMAGE
	{ IMG_Quit(); }
#endif
}

//-----------------------------------------------------------------------------
void eTextureManager::Initialize()
{
	std::ifstream infile("textures.ini");
	if (infile.is_open())
	{
		std::stringstream sstream;
		std::copy(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sstream));
		Texture text;

		std::string file_name , name, type, wrap, temp;
		std::vector<std::string> faces;
		while (!sstream.eof())
		{
			sstream >> file_name;
			sstream >> name;
			sstream >> type;
			if(type != ";")
				sstream >> wrap;

			if (!type.empty() && type != ";")
				text.type = type;

			if(type == "skybox" || type == "array" || type == "array_last")
				faces.push_back(folderPath + file_name);

			if (faces.size() == 6 && type == "skybox")
			{
				text.loadCubemap(faces);
				m_cubemap_ids.push_back(text.id);
				faces.clear();
			}
			else if (type == "array_last")
			{
				text.loadTexture2DArray(faces);
				faces.clear();
			}
			else if (!wrap.empty() && wrap != ";")
			{
				text.loadTextureFromFile(folderPath + file_name, GL_RGBA, GL_REPEAT);
				sstream >> temp; // to read ";"
			}
			else if (type == "skybox" || type == "array")
				continue;
			else if(type == "hdr")
				text.loadHdr(folderPath + file_name);
			else
				text.loadTextureFromFile(folderPath + file_name);

			if(type == "atlas4")
				text.setNumRows(4);

			m_Textures.insert(std::pair<std::string, Texture>(name, text));
		}
	}

	_LoadHardcoded();
}

//-----------------------------------------------------------------------------
void eTextureManager::InitContext(const std::string& _folderPath)
{
	folderPath = _folderPath;
#ifdef DEVIL_IMAGE
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
#endif

#ifdef SDL_IMAGE
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if(!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout << "error initing SDL Image" << std::endl;
	}
#endif
}

//-----------------------------------------------------------------------------
void eTextureManager::_LoadHardcoded()
{
	Texture text;
	text.loadTextureFromFile(folderPath + "empty.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tempty", text));
	Texture::SetEmptyTextureId(text.id);

	text.generatePerlin(600, 600, true); //HARDCODING!
	m_Textures.insert(std::pair<std::string, Texture>("Tperlin_n", text));
}

//----------------------------------------------------
const Texture* eTextureManager::Find(const std::string& texture_name) const
{
	if (m_Textures.find(texture_name) != m_Textures.end())
		return &m_Textures.find(texture_name)->second;
	else
	{
		std::cout << "texture not found!" << std::endl;
		return nullptr;
	}
}

//----------------------------------------------------
const Texture* eTextureManager::FindByID(unsigned int _id) const
{
	for (auto& texture : m_Textures)
		if (texture.second.id == _id)
			return &texture.second;
	return nullptr;
}

//----------------------------------------------------
void eTextureManager::AddExisting(const std::string& _name, Texture* _text)
{
	m_Textures.insert(std::pair<std::string, Texture>(_name, *_text));
	if(_text->type == "skybox")
		m_cubemap_ids.push_back(_text->id);
}

//----------------------------------------------------
void eTextureManager::AddTextureBox(const Texture& _texture, const std::string& _name)
{
	Texture text;
	text.makeCubemap(&(const_cast<Texture&>(_texture)));
	m_Textures.insert(std::pair<std::string, Texture>(_name, text));
	m_cubemap_ids.push_back(text.id);
}

//-----------------------------------------------------
uint64_t eTextureManager::LoadTexture(const std::string& _path, const std::string& _name, const std::string& _type)
{
	Texture text;
	text.loadTextureFromFile(_path);
	text.type = _type;
	m_Textures.insert(std::pair<std::string, Texture>(_name, text));
	return text.id;
}

