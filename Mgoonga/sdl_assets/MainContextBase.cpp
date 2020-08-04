#include "stdafx.h"

#include "MainContextBase.h"

#include <base/InputController.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <math/Clock.h>

//-----------------------------------------------------------------
eMainContextBase::eMainContextBase(eInputController* _input,
	                               IWindowImGui*,
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
{}

//-------------------------------------------------------------------------
eMainContextBase::~eMainContextBase()
{

}

//-------------------------------------------------------------------------
size_t eMainContextBase::Width() { return width; }
//-------------------------------------------------------------------------
size_t eMainContextBase::Height() { return height; }

//--------------------------------------------------------------------------
void eMainContextBase::InitializeGL()
{
	InitializeTextures();

	InitializePipline();

	InitializeBuffers();

	InitializeSounds();

	modelManager->InitializePrimitives();

	InitializeModels();

	InitializeRenders();
}

//-------------------------------------------------------------------------------
void eMainContextBase::PaintGL()
{
	static math::eClock clock;
	if (clock.isActive())
		std::cout << "FPS= " << 1000 / clock.newFrame() << std::endl;

	if (!clock.isActive())
		clock.start();
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
	modelManager->AddPrimitive("white_sphere",
		std::shared_ptr<MyModel>(new MyModel(modelManager->FindMesh("sphere"),
			texManager->Find("Twhite"))));
}

void eMainContextBase::InitializeTextures()
{
	texManager->InitContext(assetsFolderPath);
	texManager->LoadAllTextures();
	//m_Textures.Find("Tbricks0_d")->second.saveToFile("MyTexture");  //Saving texture debug
}
