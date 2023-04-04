#include "stdafx.h"

#include "MainContextBase.h"

#include <base/InputController.h>
#include <tcp_lib/Network.h>
#include <tcp_lib/Server.h>
#include <tcp_lib/Client.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <math/Clock.h>

#include <thread>

//-----------------------------------------------------------------
eMainContextBase::eMainContextBase(eInputController* _input,
	                               std::vector<IWindowImGui*> _externalGui,
	                               const std::string& _modelsPath,
	                               const std::string& _assetsPath,
	                               const std::string& _shadersPath)
: inputController(_input)
, modelFolderPath(_modelsPath)
, assetsFolderPath(_assetsPath)
, shadersFolderPath(_shadersPath)
, texManager(new eTextureManager)
, modelManager(new eModelManager)
, soundManager(new eSoundManager(_assetsPath))
, externalGui(_externalGui)
{}

//-------------------------------------------------------------------------
eMainContextBase::~eMainContextBase()
{
	tcpTimer->stop();
}

//-------------------------------------------------------------------------
size_t eMainContextBase::Width() { return width; }
//-------------------------------------------------------------------------
size_t eMainContextBase::Height() { return height; }

//--------------------------------------------------------------------------
void eMainContextBase::InitializeGL()
{
	//init main light
	m_lights.push_back({});
	m_lights[0].light_position = vec4(0.0f, 4.0f, -1.0f, 1.0f);
	m_lights[0].light_direction = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[0].type = eLightType::DIRECTION;

	//init main camera
	m_cameras.push_back(Camera(width, height, nearPlane, farPlane));
	m_cameras[0].setPosition(glm::vec3(-1.0f, 4.0f, -2.5f));
	m_cameras[0].setDirection(glm::vec3(0.6f, -0.10f, 0.8f));
	//Camera Ray
	GetMainCamera().getCameraRay().init(width, height, nearPlane, farPlane);

	InitializeTextures();

	InitializePipline();

	InitializeBuffers();

	InitializeSounds();

	modelManager->InitializePrimitives();

	InitializeModels();

	InitializeRenders();

  InitializeExternalGui();
}

//-------------------------------------------------------------------------------
void eMainContextBase::PaintGL()
{
}

//--------------------------------------------------------------------------------
uint32_t eMainContextBase::GetFinalImageId()
{
	return uint32_t();
}

//---------------------------------------------------------------------------------
std::shared_ptr<eObject> eMainContextBase::GetFocusedObject()
{
	return m_focused;
}

//--------------------------------------------------------------------------------
glm::mat4 eMainContextBase::GetMainCameraViewMatrix()
{
	return m_cameras[0].getWorldToViewMatrix();
}

//--------------------------------------------------------------------------------
glm::mat4 eMainContextBase::GetMainCameraProjectionMatrix()
{
	return m_cameras[0].getProjectionMatrix();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeModels()
{
	//PRIMITIVES
	modelManager->AddPrimitive("wall_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("cube"),
			texManager->Find("Tbrickwall0_d"),
			texManager->Find("Tbrickwall0_d"),
			texManager->Find("Tbrickwall0_n"),
			texManager->Find("Tblack"))));
	modelManager->AddPrimitive("container_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("cube"),
			texManager->Find("Tcontainer0_d"),
			texManager->Find("Tcontainer0_s"),
			texManager->Find("Tblue"),
			texManager->Find("Tblack"))));
	modelManager->AddPrimitive("arrow",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("arrow"),
			texManager->Find("Tcontainer0_d"),
			texManager->Find("Tcontainer0_s"),
			texManager->Find("Tblue"),
			texManager->Find("Tblack"))));
	modelManager->AddPrimitive("grass_plane",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("plane"),
			texManager->Find("Tgrass0_d"),
			texManager->Find("Tgrass0_d"),
			texManager->Find("Tblue"),
			texManager->Find("Tblack"))));
	modelManager->AddPrimitive("white_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("cube"),
			texManager->Find("Twhite"))));
	modelManager->AddPrimitive("brick_square",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("square"),
			texManager->Find("Tbricks0_d"),
			texManager->Find("Tbricks0_d"),
			texManager->Find("Tblue"),
			texManager->Find("Tblack"))));
	modelManager->AddPrimitive("brick_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("cube"),
			texManager->Find("Tbricks2_d"),
			texManager->Find("Tbricks2_d"),
			texManager->Find("Tbricks2_n"),
			texManager->Find("Tbricks2_dp"))));
	modelManager->AddPrimitive("pbr_cube",
	std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("cube"),
			texManager->Find("pbr1_basecolor"),
			texManager->Find("pbr1_metallic"),
			texManager->Find("pbr1_normal"),
			texManager->Find("pbr1_roughness"))));
	modelManager->AddPrimitive("white_sphere",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("sphere"),
			texManager->Find("Twhite"))));
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeTextures()
{
	texManager->InitContext(assetsFolderPath);
	texManager->LoadAllTextures();
	//m_Textures.Find("Tbricks0_d")->second.saveToFile("MyTexture");  //Saving texture debug
}

//------------------------------------------------------------
Light& eMainContextBase::GetMainLight()
{
	if (m_lights.empty())
		throw std::logic_error("main light was deleted!");

	return m_lights[0];
}
//------------------------------------------------------------------
Camera& eMainContextBase::GetMainCamera()
{
	if (m_cameras.empty())
		throw std::logic_error("main camera was deleted!");

	return m_cameras[0];
}

//--------------------------------------------------------------------------------
void eMainContextBase::InstallTcpServer()
{
	if (!tcpAgent)
	{
		if (dbb::NetWork::Initialize())
		{
			tcpAgent = std::make_unique<Server>();
			if (tcpAgent->Initialize(dbb::IPEndPoint{ "0.0.0.0", 8080 }))//134.238.94.205 //208.67.222.222
			{
				tcpTimer.reset(new math::Timer([this]()->bool
					{
						if (tcpAgent->IsConnected())
							tcpAgent->Frame();
						return true;
					}));
				tcpTimer->start(15); //~70 fps

				//this is test
				std::string msg;
				while (msg != "exit")
				{
					std::getline(std::cin, msg);
					//std::cout << "Me: " << msg << std::endl;
					tcpAgent->SendMsg(std::move(msg));
				}
			}
			dbb::NetWork::Shutdown();
		}
	}
}

//--------------------------------------------------------------------------------
void eMainContextBase::InstallTcpClient()
{
	if (!tcpAgent)
	{
		if (dbb::NetWork::Initialize())
		{
			tcpAgent = std::make_unique<Client>();
			if (tcpAgent->Initialize(dbb::IPEndPoint{ "127.0.0.1", 8080 })) //"109.95.50.27 // "192.168.2.102 //134.238.94.205 /208.67.222.222
			{
				tcpTimer.reset(new math::Timer([this]()->bool
					{
						if (tcpAgent->IsConnected())
							tcpAgent->Frame();
						return true;
					}));
				tcpTimer->start(15); //~70 fps

				//this is test
				std::string msg;
				while (msg != "exit")
				{
					std::getline(std::cin, msg);
					//std::cout << "Me: " << msg << std::endl;
					tcpAgent->SendMsg(std::move(msg));
				}
			}
			dbb::NetWork::Shutdown();
		}
	}
}

