#include "stdafx.h"
#include "SandBoxGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/BoxCollider.h>
#include <math/RigidBody.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>

#include <game_assets/ModelManagerYAML.h>
#include <game_assets/AnimationManagerYAML.h>
#include <game_assets/GUIController.h>
#include <game_assets/CameraFreeController.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ObjectFactory.h>
#include <game_assets/ShootScript.h>
#include <game_assets/AnimationSocketScript.h>

#include "SandBoxScript.h"

//-------------------------------------------------------------------------
eSandBoxGame::eSandBoxGame(eInputController*  _input,
  std::vector<IWindowImGui*> _externalGui,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
{
	ObjectPicked.Subscribe([this](shObject _new_focused, bool _left)
		{
			if (_new_focused != m_focused && _left)
			{
				FocusChanged.Occur(m_focused, _new_focused);
				m_focused = _new_focused;
				return true;
			}
			return false;
		});
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
	//modelManager->Add("MapleTree", (GLchar*)std::string(modelFolderPath + "MapleTree/MapleTree.obj").c_str());
	//modelManager->Add("Cottage", (GLchar*)std::string(modelFolderPath + "85-cottage_obj/cottage_obj.obj").c_str());

	Material material;
	material.albedo = glm::vec3(0.8f, 0.0f, 0.0f);
	material.ao = 1.0f;
	material.roughness = 0.5;
	material.metallic = 0.5;

	modelManager->Add("sphere_red", Primitive::SPHERE, std::move(material));

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
	grassPlane->GetTransform()->setScale(vec3(2.0f, 2.0f, 2.0f));
	m_objects.push_back(grassPlane);

	shObject wolf = factory.CreateObject(modelManager->Find("wolf"), eObject::RenderType::PHONG, "Wolf", "Default", "");
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(3.0f, -2.0f, 0.0f));
	wolf->GetTransform()->setScale(vec3(1.5f, 1.5f, 1.5f));
	m_objects.push_back(wolf);

	shObject soldier = factory.CreateObject(modelManager->Find("Dying"),
																					eObject::RenderType::PHONG,
																					"Soldier",
																					"Default", //"MixamoFireWalkDie.mgoongaRigger",
																					"" //"Soldier3Anim.mgoongaBoxColliderDynamic",
																					/*true*/); // dynamic collider
	soldier->GetTransform()->setTranslation(vec3(1.0f, -2.0f, 0.0f));
	soldier->GetTransform()->setScale(vec3(0.01f, 0.01f, 0.01f));
	soldier->SetScript(new AnimationSocketScript(this));

	//Set textures manually
	t.loadTextureFromFile("../game_assets/Resources/DyingSoldier/textures/Ch15_1001_Normal.png");
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
	t.loadTextureFromFile("../game_assets/Resources/DyingSoldier/textures/Ch15_1002_Normal.png");
	const_cast<I3DMesh*>(soldier->GetModel()->Get3DMeshes()[1])->AddTexture(&t);

	m_objects.push_back(soldier);
	/*modelManager->Save(soldier->GetModel(), "Soldier.mgoongaObject3d");
	modelManager->Add("Soldier", "Soldier.mgoongaObject3d");*/

	if(true)
	{
		shObject gravestone = factory.CreateObject(modelManager->Find("Gravestone"), eObject::RenderType::PBR, "Gravestone");
		gravestone->GetTransform()->setTranslation(vec3(0.5f, -2.0f, 4.0f));
		gravestone->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
		gravestone->GetTransform()->setScale(vec3(0.5f, 0.5f, 0.5f));
		const_cast<I3DMesh*>(gravestone->GetModel()->Get3DMeshes()[0])->SetMaterial(material);
		m_objects.push_back(gravestone);

		shObject tombstone = factory.CreateObject(modelManager->Find("Tombstone"), eObject::RenderType::PBR, "Tombstone");
		tombstone->GetTransform()->setTranslation(vec3(-1.5f, -2.0f, 4.0f));
		tombstone->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
		tombstone->GetTransform()->setScale(vec3(0.5f, 0.5f, 0.5f));
		const_cast<I3DMesh*>(tombstone->GetModel()->Get3DMeshes()[0])->SetMaterial(material);
		m_objects.push_back(tombstone);

		//Chest
		shObject chest = factory.CreateObject(modelManager->Find("Chest"), eObject::RenderType::PHONG, "Chest", "Default", "");
		chest->GetTransform()->setTranslation(vec3(-1.5f, -2.0f, 0.0f));
		chest->GetTransform()->setRotation(glm::radians(-90.0f), glm::radians(-90.0f), 0.0f);
		chest->GetTransform()->setScale(vec3(0.5f, 0.5f, 0.5f));
		m_objects.push_back(chest);
		chest->GetRigger()->UseFirstFrameAsIdle();
	}

	//light
	m_light_object = factory.CreateObject(modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "WhiteSphere");
	m_light_object->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
	m_objects.push_back(m_light_object);

	//GLOBAL SCRIPTS
	m_global_scripts.push_back(std::make_shared<ShootScript>(this, modelManager.get()));
	m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);

	m_global_scripts.push_back(std::make_shared<GUIController>(this, this->pipeline, soundManager->GetSound("page_sound")));
	m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

	m_input_controller->AddObserver(this, WEAK);
	m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}
