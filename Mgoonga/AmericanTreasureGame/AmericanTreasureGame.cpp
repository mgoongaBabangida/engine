#include "stdafx.h"
#include "AmericanTreasureGame.h"
#include "GameController.h"

#include <base/InputController.h>

#include <math/ParticleSystem.h>
#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxCollider.h>

#include <opengl_assets/Texture.h>
#include <opengl_assets/RenderManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <game_assets/ModelManagerYAML.h>
#include <game_assets/AnimationManagerYAML.h>
#include <game_assets/GUIController.h>
#include <game_assets/CameraFreeController.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ObjectFactory.h>
#include <game_assets/BezierCurveUIController.h>

//---------------------------------------------------------------------------
AmericanTreasureGame::AmericanTreasureGame(eInputController* _input,
                                           std::vector<IWindowImGui*> _externalGui,
                                           const std::string& _modelsPath,
                                           const std::string& _assetsPath,
                                           const std::string& _shadersPath)
  : eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
{
  FocusChanged.Subscribe([this](shObject _prev, shObject _new)->void { this->OnFocusedChanged(); });

  if (!m_objects.empty())
  {
    m_focused = m_objects[0];
    FocusChanged.Occur(shObject{}, m_focused);
  }
}

//--------------------------------------------------------------------------
AmericanTreasureGame::~AmericanTreasureGame() {}

//--------------------------------------------------------------------------
void AmericanTreasureGame::InitializeExternalGui()
{
  eMainContextBase::InitializeExternalGui();
}

//------------------------------------------------------------------------------
void AmericanTreasureGame::OnFocusedChanged()
{
}

//*********************Initialize**************************************
//-------------------------------------------------------------------------------
void AmericanTreasureGame::InitializeSounds()
{
  //sound->loadListner(GetMainCamera().getPosition().x, GetMainCamera().getPosition().y, GetMainCamera().getPosition().z); //!!!
}

//-------------------------------------------------------------------------------
void AmericanTreasureGame::InitializePipline()
{
  eMainContextBase::InitializePipline();
  pipeline.SwitchSkyBox(false);
  pipeline.SwitchWater(true);
  pipeline.GetSkyNoiseOnRef() = false;
  pipeline.GetKernelOnRef() = false;
  this->m_use_guizmo = false;
  // call all the enable pipeline functions
}

//-----------------------------------------------------------------------------
void AmericanTreasureGame::InitializeBuffers()
{
  GetMainLight().type = eLightType::DIRECTION;
  pipeline.InitializeBuffers(GetMainLight().type == eLightType::POINT);
}

//-----------------------------------------------------------------------------
void AmericanTreasureGame::InitializeModels()
{
  eMainContextBase::InitializeModels();
  
  //MODELS
  modelManager->Add("ship", (GLchar*)std::string(modelFolderPath + "Ship(low_poly)FBX.fbx").c_str());
  modelManager->Add("pirate_ship", (GLchar*)std::string(modelFolderPath + "Pirate Ship/stylized_ship(low_poly).fbx").c_str());

  Material pbr1;
  pbr1.albedo_texture_id = texManager->Find("pbr1_basecolor")->id;
  pbr1.metalic_texture_id = texManager->Find("pbr1_metallic")->id;
  pbr1.normal_texture_id = texManager->Find("pbr1_normal")->id;
  pbr1.roughness_texture_id = texManager->Find("pbr1_roughness")->id;
  pbr1.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
  pbr1.use_albedo = pbr1.use_metalic = pbr1.use_normal = pbr1.use_roughness = true;

  Material red;
  red.albedo = glm::vec3(0.9f, 0.0f, 0.0f);
  red.ao = 1.0f;
  red.roughness = 0.5;
  red.metallic = 0.5;

  modelManager->Add("sphere_textured", Primitive::SPHERE, std::move(pbr1));
  modelManager->Add("sphere_red", Primitive::SPHERE, std::move(red));

  //@todo separate init scene member func
  _InitMainTestSceane();

  //GLOBAL CONTROLLERS 
  IWindowImGui* debug_window = this->externalGui.size() > 12 ? externalGui[12] : nullptr;
  m_global_scripts.push_back(std::make_shared<GameController>(this, modelManager.get(), texManager.get(), soundManager.get(), pipeline, GetMainCamera(), debug_window));
  m_global_scripts.push_back(std::make_shared<GUIController>(this, this->pipeline, soundManager->GetSound("page_sound")));
  m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

  m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}

//-------------------------------------------------------------------------
void AmericanTreasureGame::InitializeRenders()
{
  eMainContextBase::InitializeRenders();
}

//-------------------------------------------------------------------------------
void AmericanTreasureGame::PaintGL()
{
  eMainContextBase::PaintGL();
}

//----------------------------------------------------
void AmericanTreasureGame::_InitMainTestSceane()
{
  ObjectFactoryBase factory;
  //light
  m_light_object = factory.CreateObject(modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "WhiteSphere");
  m_light_object->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
  m_objects.push_back(m_light_object);
}
