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
void eSandBoxGame::InitializeModels()
{
	eMainContextBase::InitializeModels();
	
	//MODELS
	//modelManager->Add("MapleTree", (GLchar*)std::string(modelFolderPath + "MapleTree/MapleTree.obj").c_str());
	//modelManager->Add("Cottage", (GLchar*)std::string(modelFolderPath + "85-cottage_obj/cottage_obj.obj").c_str());

	Material material{ glm::vec3(0.8f, 0.0f, 0.0f), 0.5f , 0.5f };
	material.emissive_texture_id = Texture::GetTexture1x1(TColor::BLACK).id;

	modelManager->Add("sphere_red", Primitive::SPHERE, std::move(material));

	//DESERIALIZE ANIMATIONS
	animationManager->Deserialize("Animations.mgoongaAnimations");
	
	//OBJECTS
	ObjectFactoryBase factory(animationManager.get());

	shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "WallCube");
	wallCube->GetTransform()->setTranslation(vec3(3.0f, -1.5f, 3.0f));
	//wallCube->SetScript(new eSandBoxScript(this));
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
	//grassPlane->GetModel()->SetMaterial(material);
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	grassPlane->GetTransform()->setScale(vec3(5.0f, 5.0f, 5.0f));
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
	soldier->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	soldier->GetTransform()->setScale(vec3(0.01f, 0.01f, 0.01f));
	soldier->SetScript(new AnimationSocketScript(this));

	//Set textures manually
	t.type = "texture_normal";
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
		shObject chest = factory.CreateObject(modelManager->Find("Chest"), eObject::RenderType::PBR, "Chest", "Default", "");
		chest->GetTransform()->setTranslation(vec3(-1.5f, -2.0f, 0.0f));
		chest->GetTransform()->setRotation(glm::radians(-90.0f), glm::radians(-90.0f), 0.0f);
		chest->GetTransform()->setScale(vec3(0.5f, 0.5f, 0.5f));
		m_objects.push_back(chest);
		chest->GetRigger()->UseFirstFrameAsIdle();
		Material chest_material = *(chest->GetModel()->GetMeshes()[0]->GetMaterial());
		chest_material.use_normal = true;
		chest_material.use_metalic = true;
		chest_material.use_roughness = true;
		const_cast<IMesh*>(chest->GetModel()->GetMeshes()[0])->SetMaterial(chest_material);

		t.loadTextureFromFile("../game_assets/Resources/chest/chest-diffuse.png");
		t.type = "texture_diffuse";
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[1])->AddTexture(&t);
		t.loadTextureFromFile("../game_assets/Resources/chest/chest-normal.png");
		t.type = "texture_normal";
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[1])->AddTexture(&t);
		t.loadTextureFromFile("../game_assets/Resources/chest/chest-metallic.png");
		t.type = "texture_specular";
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[1])->AddTexture(&t);
		t.loadTextureFromFile("../game_assets/Resources/chest/chest-roughness.png");
		t.type = "texture_roughness";
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[0])->AddTexture(&t);
		const_cast<I3DMesh*>(chest->GetModel()->Get3DMeshes()[1])->AddTexture(&t);

		//castle
		shObject castle = factory.CreateObject(modelManager->Find("Castle"), eObject::RenderType::PHONG, "Castle");
		castle->GetTransform()->setTranslation(vec3(1.0f, -2.0f, 2.0f));
		castle->GetTransform()->setScale(vec3(0.2f, 0.2f, 0.2f));
		m_objects.push_back(castle);
		for (auto& mesh : castle->GetModel()->Get3DMeshes())
		{
			t.loadTextureFromFile("../game_assets/assets/brickwall.jpg");
			t.type = "texture_diffuse";
			const_cast<I3DMesh*>(mesh)->AddTexture(&t);
			t.loadTextureFromFile("../game_assets/assets/brickwall_normal.jpg");
			t.type = "texture_normal";
			const_cast<I3DMesh*>(mesh)->AddTexture(&t);
			mesh->GetMaterial()->use_normal = true;
			/*const_cast<I3DMesh*>(mesh)->calculatedTangent();
			const_cast<I3DMesh*>(mesh)->ReloadVertexBuffer();*/
		}
	}

	//light
	pipeline.SetUniformData("class ePhongRender","emission_strength", 5.0f);
	shObject hdr_object = factory.CreateObject(modelManager->Find("white_quad"), eObject::RenderType::PHONG, "LightObject"); // or "white_quad"
	if(hdr_object->GetModel()->GetName() == "white_sphere")
		hdr_object->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	hdr_object->GetTransform()->setTranslation(GetMainLight().light_position);
	m_light_object = hdr_object;
	std::array<glm::vec4, 4> points = { // for area light
		glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) };
	GetMainLight().points = points;

	Material m { vec3{}, 0.0f, 0.0f, 1.0f, 
		Texture::GetTexture1x1(TColor::YELLOW).id, Texture::GetTexture1x1(TColor::WHITE).id, 
		Texture::GetTexture1x1(TColor::BLUE).id, Texture::GetTexture1x1(TColor::WHITE).id, Texture::GetTexture1x1(TColor::YELLOW).id,
	true, true, true, true};

	hdr_object->GetModel()->SetMaterial(m);
	m_objects.push_back(hdr_object);

	//GLOBAL SCRIPTS
	m_global_scripts.push_back(std::make_shared<ShootScript>(this, modelManager.get()));
	m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);

	m_global_scripts.push_back(std::make_shared<GUIControllerBase>(this, this->pipeline, soundManager->GetSound("page_sound")));
	m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

	m_input_controller->AddObserver(this, WEAK);
	m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}
