#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"
#include <iostream>

#define DEVIL_IMAGE
#define SDL_IMAGE

#ifdef DEVIL_IMAGE
	#include "TextureImplDevIl.h"
#endif

#ifdef SDL_IMAGE
	#include "TextureImplSDL.h"
#endif

eTextureManager::~eTextureManager()
{
  for (auto& node : m_Textures)
    glDeleteTextures(1, &node.second.id);

#ifdef SDL_IMAGE
	{ IMG_Quit(); }
#endif
}

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

void eTextureManager::LoadAllTextures()
{
	Texture text;
	text.loadTextureFromFile(folderPath + "bricks.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tbricks0_d", text));
	
	text.loadTextureFromFile(folderPath + "container2.png");
	m_Textures.insert(std::pair<std::string, Texture>("Tcontainer0_d", text));
	
	text.loadTextureFromFile(folderPath + "grass.png");
	m_Textures.insert(std::pair<std::string, Texture>("Tgrass0_d", text));
	
	text.loadTextureFromFile(folderPath + "container2_specular.png");
	m_Textures.insert(std::pair<std::string, Texture>("Tcontainer0_s", text));
	
	text.loadTexture1x1(WHITE);
	m_Textures.insert(std::pair<std::string, Texture>("Twhite", text));
	
	text.loadTexture1x1(BLACK);
	m_Textures.insert(std::pair<std::string, Texture>("Tblack", text));
	
	text.loadTexture1x1(BLUE);
	m_Textures.insert(std::pair<std::string, Texture>("Tblue", text));
	
	text.loadTexture1x1(PINK);
	m_Textures.insert(std::pair<std::string, Texture>("Tpink", text));
	
	text.loadTextureFromFile(folderPath + "brickwall.jpg");//"brickwall.jpg"
	m_Textures.insert(std::pair<std::string, Texture>("Tbrickwall0_d", text));
	
	text.loadTextureFromFile(folderPath + "brickwall_normal.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tbrickwall0_n", text));
	
	text.loadTextureFromFile(folderPath + "heightmap.png");
	m_Textures.insert(std::pair<std::string, Texture>("Theightmap0_n", text));
	text.loadTextureFromFile(folderPath + "waterDuDV.png");
	m_Textures.insert(std::pair<std::string, Texture>("TwaterDUDV", text));
	text.loadTextureFromFile(folderPath + "DUDVwaves.png");//, GL_RGBA, GL_REPEAT
	m_Textures.insert(std::pair<std::string, Texture>("Twaves0_n", text));
	text.loadTextureFromFile(folderPath + "heightmap.png");
	m_Textures.insert(std::pair<std::string, Texture>("Theightmap0_n", text));
	text.generatePerlin(600, 600, true); //HARDCODING!
	m_Textures.insert(std::pair<std::string, Texture>("Tperlin_n", text));
	text.loadTextureFromFile(folderPath + "spanish.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("TSpanishFlag0_s", text));
	text.loadTextureFromFile(folderPath + "atlas2.png");
	m_Textures.insert(std::pair<std::string, Texture>("TfireAtlas0_n", text));
	text.loadTextureFromFile(folderPath + "shooting.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("TShootAtlas0_n", text));
	text.loadTextureFromFile(folderPath + "penguins.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tpenguins", text));
	text.loadTextureFromFile(folderPath + "bricks2.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tbricks2_d", text));
	text.loadTextureFromFile(folderPath + "bricks2_normal.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tbricks2_n", text));
	text.loadTextureFromFile(folderPath + "bricks2_depth.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("Tbricks2_dp", text));
	text.loadTextureFromFile(folderPath + "pirate.png");
	m_Textures.insert(std::pair<std::string, Texture>("TPirate_flag0_s", text));
	text.loadTextureFromFile(folderPath + "ocean1.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("TOcean0_s", text));

	text.loadTextureFromFile(folderPath + "dice1.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice1", text));
	text.loadTextureFromFile(folderPath + "dice2.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice2", text));
	text.loadTextureFromFile(folderPath + "dice3.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice3", text));
	text.loadTextureFromFile(folderPath + "dice4.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice4", text));
	text.loadTextureFromFile(folderPath + "dice5.jpg");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice5", text));
	text.loadTextureFromFile(folderPath + "dice6.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice6", text));
	
	text.loadTextureFromFile(folderPath + "bloom_test.jpg");
	text.setNumRows(4);
	m_Textures.insert(std::pair<std::string, Texture>("Tbloom_test", text));

	// Cubemap (Skybox)
	std::vector<std::string> faces;
	faces.push_back(folderPath + "right.jpg");
	faces.push_back(folderPath + "left.jpg");
	faces.push_back(folderPath + "top.jpg");
	faces.push_back(folderPath + "bottom.jpg");
	faces.push_back(folderPath + "back.jpg");
	faces.push_back(folderPath + "front.jpg");
	text.loadCubemap(faces);
	m_Textures.insert(std::pair<std::string, Texture>("TcubeSkyWater1", text));

	std::vector<std::string> faces2;
	faces2.push_back(folderPath + "envmap_miramar/miramar_rt.tga");
	faces2.push_back(folderPath + "envmap_miramar/miramar_lf.tga");
	faces2.push_back(folderPath + "envmap_miramar/miramar_dn.tga");
	faces2.push_back(folderPath + "envmap_miramar/miramar_up.tga");
	faces2.push_back(folderPath + "envmap_miramar/miramar_bk.tga");
	faces2.push_back(folderPath + "envmap_miramar/miramar_ft.tga");
	text.loadCubemap(faces2);
	m_Textures.insert(std::pair<std::string, Texture>("TcubeSkyWater2", text));

	text.loadTextureFromFile(folderPath + "atlas2.png");
	text.setNumRows(4);
	m_Textures.insert(std::pair<std::string, Texture>("Tatlas2", text));
}

Texture* eTextureManager::Find(const std::string& texture_name)
{
	if (m_Textures.find(texture_name) != m_Textures.end())
		return &m_Textures.find(texture_name)->second;
	else {
		std::cout << "texture not found!" << std::endl;
		return nullptr;
	}
}

void eTextureManager::AddTextureBox(const Texture& _texture, const std::string& _name)
{
	Texture text;
	text.makeCubemap(&(const_cast<Texture&>(_texture)));
	m_Textures.insert(std::pair<std::string, Texture>(_name, text));
}
