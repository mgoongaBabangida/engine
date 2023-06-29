#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"

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
	
	text.type = "terrain"; //@todo
	text.loadTextureFromFile(folderPath + "grass.png", GL_RGBA, GL_REPEAT);
	m_Textures.insert(std::pair<std::string, Texture>("Tgrass0_d", text));
	
	text.type = "";
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
	text.loadTextureFromFile(folderPath + "waterDuDV.png", GL_RGBA, GL_REPEAT);
	m_Textures.insert(std::pair<std::string, Texture>("TwaterDUDV", text));
	text.loadTextureFromFile(folderPath + "DUDVwaves.png", GL_RGBA, GL_REPEAT);
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
	text.loadTextureFromFile(folderPath + "red_button.png");
	m_Textures.insert(std::pair<std::string, Texture>("TButton_red", text));
	text.loadTextureFromFile(folderPath + "transparent-michelangelo.png");
	m_Textures.insert(std::pair<std::string, Texture>("Michelangelo", text));
	text.loadTextureFromFile(folderPath + "cursor1.png");
	m_Textures.insert(std::pair<std::string, Texture>("cursor1", text));
	text.loadTextureFromFile(folderPath + "menu2.png");
	m_Textures.insert(std::pair<std::string, Texture>("menu2", text));
	text.loadTextureFromFile(folderPath + "menu3.png");
	m_Textures.insert(std::pair<std::string, Texture>("menu3", text));
	text.loadTextureFromFile(folderPath + "ship_icon.png");
	m_Textures.insert(std::pair<std::string, Texture>("ship1", text));

	text.loadTextureFromFile(folderPath + "dice1.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice1", text));
	text.loadTextureFromFile(folderPath + "dice2.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice2", text));
	text.loadTextureFromFile(folderPath + "dice3.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice3", text));
	text.loadTextureFromFile(folderPath + "dice4.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice4", text));
	text.loadTextureFromFile(folderPath + "dice5.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice5", text));
	text.loadTextureFromFile(folderPath + "dice6.png");
	m_Textures.insert(std::pair<std::string, Texture>("tex_dice6", text));

	text.loadTextureFromFile(folderPath + "red-circle.png");
	m_Textures.insert(std::pair<std::string, Texture>("red_circle", text));
	text.loadTextureFromFile(folderPath + "green-circle.png");
	m_Textures.insert(std::pair<std::string, Texture>("green_circle", text));

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

	//pbr sphere
  text.loadTextureFromFile(folderPath + "sphere_pbr1/rustediron2_basecolor.png");
  m_Textures.insert(std::pair<std::string, Texture>("pbr1_basecolor", text));
  text.loadTextureFromFile(folderPath + "sphere_pbr1/rustediron2_metallic.png");
  m_Textures.insert(std::pair<std::string, Texture>("pbr1_metallic", text));
  text.loadTextureFromFile(folderPath + "sphere_pbr1/rustediron2_normal.png");
  m_Textures.insert(std::pair<std::string, Texture>("pbr1_normal", text));
  text.loadTextureFromFile(folderPath + "sphere_pbr1/rustediron2_roughness.png");
  m_Textures.insert(std::pair<std::string, Texture>("pbr1_roughness", text));
}

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
void eTextureManager::AddTextureBox(const Texture& _texture, const std::string& _name)
{
	Texture text;
	text.makeCubemap(&(const_cast<Texture&>(_texture)));
	m_Textures.insert(std::pair<std::string, Texture>(_name, text));
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

