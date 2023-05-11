#include "stdafx.h"
#include "SandBoxGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>
#include <math/ParticleSystem.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <game_assets/ModelManagerYAML.h>
#include <game_assets/AnimationManagerYAML.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ObjectFactory.h>

#include "SandBoxScript.h"

//-------------------------------------------------------------------------
eSandBoxGame::eSandBoxGame(eInputController*  _input,
  std::vector<IWindowImGui*> _externalGui,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
{
}

void eSandBoxGame::PaintGL()
{
	eMainContextBase::PaintGL();
}

//*********************InputObserver*********************************
bool eSandBoxGame::OnMouseMove(uint32_t x, uint32_t y)
{
	return false;
}

//------------------------------------------------------------------
bool eSandBoxGame::OnKeyPress(uint32_t asci)
{
	if (eMainContextBase::OnKeyPress(asci))
		return true;

	switch (asci)
	{
	case ASCII_G:
	{
		if (m_focused != nullptr)
			m_focused->GetScript()->OnKeyPress(ASCII_G);
	}
	return true;
	default: return false;
	}
}

//------------------------------------------------------------------
bool eSandBoxGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	GetMainCamera().getCameraRay().Update(GetMainCamera(), x, y, static_cast<float>(width), static_cast<float>(height));
	if (left)
	{
		GetMainCamera().getCameraRay().press(x, y);
		m_focused = GetMainCamera().getCameraRay().calculateIntersaction(m_objects).first;
	}
	if (m_focused && m_focused->GetScript())
	{
		m_focused->GetScript()->OnMousePress(x, y, left);
	}
	return true;
}

//------------------------------------------------------------------
bool eSandBoxGame::OnMouseRelease()
{
	GetMainCamera().getCameraRay().release();
	return true;
}

//*********************Initialize**************************************
void eSandBoxGame::InitializePipline()
{
	pipeline.Initialize();
	pipeline.SwitchSkyBox(false);
	pipeline.SwitchWater(false);
	pipeline.GetSkyNoiseOnRef() = false;
	pipeline.GetKernelOnRef() = false;
	this->m_use_guizmo = false;
	// call all the enable pipeline functions
}

void eSandBoxGame::InitializeBuffers()
{
	GetMainLight().type = eLightType::DIRECTION;
	pipeline.InitializeBuffers(GetMainLight().type == eLightType::POINT); //@todo add possibility to choose buffers
}

void eSandBoxGame::InitializeModels()
{
	eMainContextBase::InitializeModels();
	
	//MODELS
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager->Add("Dying", (GLchar*)std::string(modelFolderPath + "Dying Soldier/Dying.dae").c_str());

	//DESERIALIZE ANIMATIONS
	animationManager->Deserialize("Animations.mgoongaAnimations");
	
	//OBJECTS
	ObjectFactoryBase factory(animationManager.get());

	shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "WallCube");
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	wallCube->SetScript(new eSandBoxScript());
	m_objects.push_back(wallCube);
	
	shObject grassPlane = factory.CreateObject(modelManager->Find("grass_plane"), eObject::RenderType::PHONG, "GrassPlane");
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_objects.push_back(grassPlane);

	shObject wolf = factory.CreateObject(modelManager->Find("wolf"), eObject::RenderType::PHONG, "Wolf", "Default", "");
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(3.0f, -2.0f, 0.0f));
	wolf->GetTransform()->setScale(vec3(1.5f, 1.5f, 1.5f));
	m_objects.push_back(wolf);

	shObject soldier = factory.CreateObject(modelManager->Find("Dying"),
																					eObject::RenderType::PHONG,
																					"Soldier",
																					"MixamoFireWalkDie.mgoongaRigger",
																					"Soldier3Anim.mgoongaBoxColliderDynamic",
																					true); // dynamic collider
	soldier->GetTransform()->setTranslation(vec3(1.0f, -2.0f, 0.0f));
	soldier->GetTransform()->setScale(vec3(0.01f, 0.01f, 0.01f));

	//Set textures manually
	Texture* t = texManager->FindByID(texManager->LoadTexture("../game_assets/Resources/Dying Soldier/textures/Ch15_1001_Normal.png", "soldier_normal1", "texture_normal"));
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[0])->AddTexture(t);
	t = texManager->FindByID(texManager->LoadTexture("../game_assets/Resources/Dying Soldier/textures/Ch15_1002_Normal.png", "soldier_normal2", "texture_normal"));
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[1])->AddTexture(t);

	m_objects.push_back(soldier);
	/*modelManager->Save(soldier->GetModel(), "Soldier.mgoongaObject3d");
	modelManager->Add("Soldier", "Soldier.mgoongaObject3d");*/

	//light
	m_light_object = factory.CreateObject(modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "WhiteSphere");
	m_light_object->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
	m_objects.push_back(m_light_object);

	//gui
	Texture* cursor = texManager->Find("cursor1");
	m_guis.emplace_back(new Cursor(0, 0, 30, 30, width, height));
	m_guis[0]->SetTexture(*cursor, { 0,0 }, { cursor->mTextureWidth, cursor->mTextureHeight });

	inputController->AddObserver(m_guis[0].get(), WEAK);
	inputController->AddObserver(this, WEAK);
	inputController->AddObserver(&GetMainCamera().getCameraRay(), WEAK);
	inputController->AddObserver(&GetMainCamera(), WEAK);
}

void eSandBoxGame::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
}

void eSandBoxGame::InitializeSounds()
{
}

//**********************************External Gui*****************************
void eSandBoxGame::InitializeExternalGui()
{
	eMainContextBase::InitializeExternalGui();
}
