#include "TextureManager.h"
#include <iostream>

#define DEVIL_IMAGE
#define SDL_IMAGE

#ifdef DEVIL_IMAGE
	#include "TextureImplDevIl.h"
#endif

#ifdef SDL_IMAGE
	#include "TextureImplSDL.h"
#endif

TextureManager::~TextureManager()
{
#ifdef SDL_IMAGE
	{ IMG_Quit(); }
#endif
}

void TextureManager::InitContext(const std::string& _folderPath)
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

void TextureManager::loadAllTextures()
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
	text.generatePerlin(255, 255, true); //HARDCODING!
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
	text.loadTextureFromFile(folderPath + "atlas2.png");
	text.setNumRows(4);
	m_Textures.insert(std::pair<std::string, Texture>("Tatlas2", text));
}

Texture* TextureManager::find(const std::string& texture_name)
{
	if (m_Textures.find(texture_name) != m_Textures.end())
		return &m_Textures.find(texture_name)->second;
	else {
		std::cout << "texture not found!" << std::endl;
		return nullptr;
	}
}
