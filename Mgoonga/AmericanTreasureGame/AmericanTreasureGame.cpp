#include "stdafx.h"
#include "AmericanTreasureGame.h"
#include "GameController.h"

#include <base/InputController.h>

#include <math/ParticleSystem.h>
#include <math/Rigger.h>
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
                                           std::vector<IWindowImGui*>& _externalGui,
                                           const std::string& _modelsPath,
                                           const std::string& _assetsPath,
                                           const std::string& _shadersPath,
                                           int _width,
                                           int _height)
  : eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath, _width, _height)
{
}

//--------------------------------------------------------------------------
AmericanTreasureGame::~AmericanTreasureGame() {}

//-----------------------------------------------------------------------------
void AmericanTreasureGame::InitializeModels()
{
  eMainContextBase::InitializeModels();
  
  //MODELS

  Material pbr1;
  pbr1.albedo_texture_id = texManager->Find("pbr1_basecolor")->m_id;
  pbr1.metalic_texture_id = texManager->Find("pbr1_metallic")->m_id;
  pbr1.normal_texture_id = texManager->Find("pbr1_normal")->m_id;
  pbr1.roughness_texture_id = texManager->Find("pbr1_roughness")->m_id;
  pbr1.emissive_texture_id = Texture::GetTexture1x1(BLACK).m_id;
  pbr1.use_albedo = pbr1.use_metalic = pbr1.use_normal = pbr1.use_roughness = true;

  Material red;
  red.albedo = glm::vec3(0.9f, 0.0f, 0.0f);
  red.ao = 1.0f;
  red.roughness = 0.5;
  red.metallic = 0.5;

  modelManager->Add("sphere_textured", Primitive::SPHERE, std::move(pbr1));
  modelManager->Add("sphere_red", Primitive::SPHERE, std::move(red));

  //GLOBAL CONTROLLERS 
  IWindowImGui* debug_window = this->externalGui.size() > 12 ? externalGui[12] : nullptr;
  m_global_scripts.push_back(std::make_shared<GameController>(this, modelManager.get(), texManager.get(), soundManager.get(), pipeline, GetMainCamera(), debug_window));
  m_global_scripts.push_back(std::make_shared<GUIController>(this, this->pipeline, soundManager->GetSound("page_sound")));
  m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera(), true));

  m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}
