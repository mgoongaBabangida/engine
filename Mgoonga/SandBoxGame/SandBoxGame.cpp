#include "stdafx.h"
#include "SandBoxGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>

#include <game_assets/ModelManagerYAML.h>
#include <game_assets/AnimationManagerYAML.h>
#include <game_assets/GUIController.h>
#include <game_assets/CameraFreeController.h>

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

//*********************InputObserver*********************************
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
			m_focused->GetScript()->OnKeyPress(ASCII_G); // change -> subscribe directly
	}
	return true;
	default: return false;
	}
}

//------------------------------------------------------------------
bool eSandBoxGame::OnMousePress(int32_t x, int32_t y, bool left)
{
	bool ret = eMainContextBase::OnMousePress(x, y, left);

	auto [new_focused, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);
	if (new_focused != m_focused)
	{
		FocusChanged.Occur(m_focused, new_focused);
		m_focused = new_focused;
		return true;
	}
	return ret;
}

//*********************Initialize**************************************
void eSandBoxGame::InitializePipline()
{
	eMainContextBase::InitializePipline();
	pipeline.SwitchSkyBox(false);
	pipeline.SwitchWater(false);
	pipeline.GetSkyNoiseOnRef() = false;
	pipeline.GetKernelOnRef() = false;
	this->m_use_guizmo = false;
	// call all the enable pipeline functions
}

//-------------------------------------------------------------------------
void eSandBoxGame::InitializeBuffers()
{
	eMainContextBase::InitializeBuffers();
	GetMainLight().type = eLightType::DIRECTION;
	pipeline.InitializeBuffers(GetMainLight().type == eLightType::POINT); //@todo add possibility to choose buffers
}

//-------------------------------------------------------------------------
void eSandBoxGame::InitializeModels()
{
	eMainContextBase::InitializeModels();
	
	//MODELS
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager->Add("Dying", (GLchar*)std::string(modelFolderPath + "Dying Soldier/Dying.dae").c_str());
	modelManager->Add("MapleTree", (GLchar*)std::string(modelFolderPath + "MapleTree/MapleTree.obj").c_str());
	modelManager->Add("Cottage", (GLchar*)std::string(modelFolderPath + "85-cottage_obj/cottage_obj.obj").c_str());

	Material red;
	red.albedo = glm::vec3(0.9f, 0.0f, 0.0f);
	red.ao = 1.0f;
	red.roughness = 1.0f;
	red.metallic = 0.0f;
	modelManager->Add("sphere_red", Primitive::SPHERE, std::move(red));

	//DESERIALIZE ANIMATIONS
	animationManager->Deserialize("Animations.mgoongaAnimations");
	
	//OBJECTS
	ObjectFactoryBase factory(animationManager.get());

	shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "WallCube");
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	wallCube->SetScript(new eSandBoxScript(this));
	m_objects.push_back(wallCube);
	
	if (false) // mapleTree
	{
		shObject mapleTree = factory.CreateObject(modelManager->Find("MapleTree"), eObject::RenderType::PHONG, "MapleTree");
		mapleTree->GetTransform()->setTranslation(vec3(3.0f, -2.0f, -2.0f));
		mapleTree->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
		m_objects.push_back(mapleTree);
	}

	Texture t;
	if (false) //cottage
	{
		shObject cottage = factory.CreateObject(modelManager->Find("Cottage"), eObject::RenderType::PHONG, "Cottage");
		cottage->GetTransform()->setTranslation(vec3(0.5f, -2.01f, -2.0f));
		cottage->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
		m_objects.push_back(cottage);
		t.loadTextureFromFile("../game_assets/Resources/85-cottage_obj/cottage_diffuse.png");
		for (auto& mesh : cottage->GetModel()->Get3DMeshes())
			const_cast<I3DMesh*>(mesh)->AddTexture(&t);

		t.loadTextureFromFile("../game_assets/Resources/85-cottage_obj/cottage_normal.png");
		for (auto& mesh : cottage->GetModel()->Get3DMeshes())
			const_cast<I3DMesh*>(mesh)->AddTexture(&t);
	}

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
	t.loadTextureFromFile("../game_assets/Resources/Dying Soldier/textures/Ch15_1001_Normal.png");
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
	t.loadTextureFromFile("../game_assets/Resources/Dying Soldier/textures/Ch15_1002_Normal.png");
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[1])->AddTexture(&t);

	m_objects.push_back(soldier);
	/*modelManager->Save(soldier->GetModel(), "Soldier.mgoongaObject3d");
	modelManager->Add("Soldier", "Soldier.mgoongaObject3d");*/

	//light
	m_light_object = factory.CreateObject(modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "WhiteSphere");
	m_light_object->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
	m_objects.push_back(m_light_object);

	m_global_scripts.push_back(std::make_shared<GUIController>(this, this->pipeline, soundManager->GetSound("page_sound")));
	m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

	m_input_controller->AddObserver(this, WEAK);
	m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}

//------------------------------------------------------------------------
void eSandBoxGame::InitializeRenders()
{
	eMainContextBase::InitializeRenders();
	// set uniforms
	// exposure, shininess etc. @todo dont change every frame in render
}

//------------------------------------------------------------------------
void eSandBoxGame::InitializeSounds()
{
	eMainContextBase::InitializeSounds();
	//@todo transfer from inside the manager
}
