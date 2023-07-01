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

//*********************InputObserver*********************************
//--------------------------------------------------------------------------
bool AmericanTreasureGame::OnKeyPress(uint32_t asci)
{
  return eMainContextBase::OnKeyPress(asci);
}

//--------------------------------------------------------------------------
bool AmericanTreasureGame::OnMouseMove(int32_t x, int32_t y)
{
  if (GetMainCamera().getCameraRay().IsPressed())
  {
    if (m_input_strategy && !m_input_strategy->OnMouseMove(x, y))
    {
      // input strategy has priority over frame, @todo frmae should be inside one of input strategies
      m_framed.reset(new std::vector<shObject>(GetMainCamera().getCameraRay().onMove(GetMainCamera(), m_objects, static_cast<float>(x), static_cast<float>(y)))); 	//to draw a frame
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------
bool AmericanTreasureGame::OnMousePress(int32_t x, int32_t y, bool left)
{
  if (m_framed)
    m_framed->clear();

  GetMainCamera().getCameraRay().Update(GetMainCamera(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
  GetMainCamera().getCameraRay().press(x, y);

  //should be inside input strategy which needs it(frame, moveXZ)
  GetMainCamera().MovementSpeedRef() = 0.f;

  if (left)
  {
    //Get Visible and Children
    auto [picked, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);
    if (picked != m_focused)
      ObjectPicked.Occur(picked);
  }

  if (m_input_strategy)
    m_input_strategy->OnMousePress(x, y, left);

  return false;
}

//---------------------------------------------------------------------------------
bool AmericanTreasureGame::OnMouseRelease()
{
  GetMainCamera().getCameraRay().release();
  if (m_input_strategy)
    m_input_strategy->OnMouseRelease();
  //should be inside input strategy which needs it(frame, moveXZ)
  GetMainCamera().MovementSpeedRef() = 0.05f;
  return true;
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
  modelManager->Add("ship", (GLchar*)std::string(modelFolderPath + "Cabin_cruise/19291_Cabin_cruise_v2_NEW.obj").c_str());

  std::vector<const Texture*> textures{ texManager->Find("pbr1_basecolor"),
                                        texManager->Find("pbr1_metallic"),
                                        texManager->Find("pbr1_normal"),
                                        texManager->Find("pbr1_roughness") };

  modelManager->Add("sphere_textured", textures /*std::vector<const Texture*>{}*/); // or textures
  modelManager->Add("sphere_red");//@todo

  //@todo separate init scene member func
  _InitMainTestSceane();

  //GLOBAL CONTROLLERS
  m_global_scripts.push_back(std::make_shared<GameController>(this, modelManager.get(), texManager.get(), soundManager.get(), pipeline, GetMainCamera()));
  m_global_scripts.push_back(std::make_shared<GUIController>(this, this->pipeline, soundManager->GetSound("page_sound")));
  m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

  m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}

//-------------------------------------------------------------------------
void AmericanTreasureGame::InitializeRenders()
{
  eMainContextBase::InitializeRenders();
  /*pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                                   texManager->Find("Tatlas2"),
                                                                   soundManager->GetSound("shot_sound"),
                                                                   texManager->Find("Tatlas2")->numberofRows));*/
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
